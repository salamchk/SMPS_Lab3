#include "bmp280.h"
//------------------------------------------------
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
char str1[100];
BMP280_CalibData CalibData;
int32_t temper_int;
void Error(void)
{
	LED_OFF;
}

static void I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 0x10000);
  if(status != HAL_OK) Error();
}


static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 0x10000);
  if(status != HAL_OK) Error();
  return value;
}

static void I2Cx_ReadData16(uint16_t Addr, uint8_t Reg, uint16_t *Value)
{

  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 2, 0x10000);
  if(status != HAL_OK) Error();
}
//------------------------------------------------

static void I2Cx_ReadData24(uint16_t Addr, uint8_t Reg, uint32_t *Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 3, 0x10000);
  if(status != HAL_OK) Error();
}

void BMP280_WriteReg(uint8_t Reg, uint8_t Value)
{
  I2Cx_WriteData(BMP280_ADDRESS, Reg, Value);
}

uint8_t BMP280_ReadReg(uint8_t Reg)
{
  uint8_t res = I2Cx_ReadData(BMP280_ADDRESS,Reg);
  return res;
}

void BMP280_ReadReg_U16(uint8_t Reg, uint16_t *Value)
{
  I2Cx_ReadData16(BMP280_ADDRESS,Reg,Value);
}

void BMP280_ReadReg_S16(uint8_t Reg, int16_t *Value)
{
  I2Cx_ReadData16(BMP280_ADDRESS,Reg, (uint16_t*) Value);
}

uint8_t BMP280_ReadStatus(void)
{
  //clear unuset bits
  uint8_t res = BMP280_ReadReg(BMP280_REGISTER_STATUS)&0x09;
  return res;
}

void BME280_ReadCoefficients(void)
{
  BMP280_ReadReg_U16(BMP280_REGISTER_DIG_T1,&CalibData.dig_T1);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_T2,&CalibData.dig_T2);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_T3,&CalibData.dig_T3);
  BMP280_ReadReg_U16(BMP280_REGISTER_DIG_P1,&CalibData.dig_P1);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P2,&CalibData.dig_P2);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P3,&CalibData.dig_P3);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P4,&CalibData.dig_P4);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P5,&CalibData.dig_P5);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P6,&CalibData.dig_P6);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P7,&CalibData.dig_P7);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P8,&CalibData.dig_P8);
  BMP280_ReadReg_S16(BMP280_REGISTER_DIG_P9,&CalibData.dig_P9);
}

void BMP280_SetStandby(uint8_t tsb) 
{
  uint8_t reg;
  reg = BMP280_ReadReg(BMP280_REG_CONFIG) & ~BMP280_STBY_MSK;
  reg |= tsb & BMP280_STBY_MSK;
  BMP280_WriteReg(BMP280_REG_CONFIG,reg);
}

void BME280_SetFilter(uint8_t filter)
{
  uint8_t reg;
  reg = BMP280_ReadReg(BMP280_REG_CONFIG) & ~BMP280_FILTER_MSK;
  reg |= filter & BMP280_FILTER_MSK;
  BMP280_WriteReg(BMP280_REG_CONFIG,reg);
}
void BMP280_SetOversamplingTemper(uint8_t osrs)
{
  uint8_t reg;
  reg = BMP280_ReadReg(BMP280_REG_CTRL_MEAS) & ~BMP280_OSRS_T_MSK;
  reg |= osrs & BMP280_OSRS_T_MSK;
  BMP280_WriteReg(BMP280_REG_CTRL_MEAS,reg);
}

void BMP280_SetOversamplingPressure(uint8_t osrs)
{
  uint8_t reg;
  reg = BMP280_ReadReg(BMP280_REG_CTRL_MEAS) & ~BMP280_OSRS_P_MSK;
  reg |= osrs & BMP280_OSRS_P_MSK;
  BMP280_WriteReg(BMP280_REG_CTRL_MEAS,reg);
}
void BMP280_SetMode(uint8_t mode) 
{
  uint8_t reg;
  reg = BMP280_ReadReg(BMP280_REG_CTRL_MEAS) & ~BMP280_MODE_MSK;
  reg |= mode & BMP280_MODE_MSK;
  BMP280_WriteReg(BMP280_REG_CTRL_MEAS,reg);
}



void BMP280_ReadReg_U24(uint8_t Reg, uint32_t *Value)
{
  I2Cx_ReadData24(BMP280_ADDRESS,Reg,Value);
  *(uint32_t *) Value &= 0x00FFFFFF;
}

void BMP280_ReadReg_BE_U24(uint8_t Reg, uint32_t *Value)
{
  I2Cx_ReadData24(BMP280_ADDRESS,Reg,Value);
  *(uint32_t *) Value = be24toword(*(uint32_t *) Value) & 0x00FFFFFF;
}

float BMP280_ReadTemperature(void)
{
  float temper_float = 0.0f;
  int32_t val1, val2;
	uint32_t temper_raw;
	BMP280_ReadReg_BE_U24(BMP280_REGISTER_TEMPDATA,&temper_raw);
	temper_raw >>= 4;
	val1 = ((((temper_raw>>3) - ((int32_t)CalibData.dig_T1 <<1))) *
((int32_t)CalibData.dig_T2)) >> 11;
val2 = (((((temper_raw>>4) - ((int32_t)CalibData.dig_T1)) *
((temper_raw>>4) - ((int32_t)CalibData.dig_T1))) >> 12) *
((int32_t)CalibData.dig_T3)) >> 14;
temper_int = val1 + val2;
	temper_float = ((temper_int * 5 + 128) >> 8);
temper_float /= 100.0f;
	
	return temper_float;
}

float BMP280_ReadPressure(void)
{
	float press_float = 0.0f;
	uint32_t press_raw, pres_int;
	int64_t val1, val2, p;
	BMP280_ReadTemperature(); // must be done first to get t_fine
	BMP280_ReadReg_BE_U24(BMP280_REGISTER_PRESSUREDATA,&press_raw);
	press_raw >>= 4;
	val1 = ((int64_t) temper_int) - 128000;
	val2 = val1 * val1 * (int64_t)CalibData.dig_P6;
	val2 = val2 + ((val1 * (int64_t)CalibData.dig_P5) << 17);
	val2 = val2 + ((int64_t)CalibData.dig_P4 << 35);
	val1 = ((val1 * val1 * (int64_t)CalibData.dig_P3) >> 8) + ((val1 * (int64_t)CalibData.dig_P2) << 12);
	val1 = (((((int64_t)1) << 47) + val1)) * ((int64_t)CalibData.dig_P1) >> 33;
	if (val1 == 0) 
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - press_raw;
	p = (((p << 31) - val2) * 3125) / val1;
	val1 = (((int64_t)CalibData.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	val2 = (((int64_t)CalibData.dig_P8) * p) >> 19;
	p = ((p + val1 + val2) >> 8) + ((int64_t)CalibData.dig_P7 << 4);
	pres_int = ((p >> 8) * 1000) + (((p & 0xff) * 390625) / 100000);
	press_float = pres_int / 100.0f;
	return press_float;
}

//------------------------------------------------
void BMP280_Init(void)
{
	uint8_t value = 0;
	uint32_t value32 = 0;
	LED_ON;
	value = BMP280_ReadReg(BMP280_REG_ID);
	if(value !=BMP280_ID)
	{
		Error();
		return;
	}
	BMP280_WriteReg(BMP280_REG_SOFTRESET,BMP280_SOFTRESET_VALUE);
	while (BMP280_ReadStatus() & BMP280_STATUS_IM_UPDATE) ;
	BME280_ReadCoefficients();
	BMP280_SetStandby(BMP280_STBY_1000); 
	BME280_SetFilter(BMP280_FILTER_4);
	BMP280_SetOversamplingTemper(BMP280_OSRS_T_x4);
	BMP280_SetOversamplingPressure(BMP280_OSRS_P_x2);
	value32 = BMP280_ReadReg(BMP280_REG_CTRL_MEAS);
	BMP280_SetMode(BMP280_MODE_NORMAL);
}
