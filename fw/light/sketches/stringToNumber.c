#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "stringToNumber.h"

// функция пытается найти число в строке и вернуть его начальную и конечную позиции
static bool findBounds(const char * const pText, size_t textLen, size_t * const pStart, size_t * const pEnd)
{
	enum
	{
		space,
		number,
		finished
	} state = space;

	size_t start;
	size_t end;

	for (size_t i = 0; i < textLen; ++i)
	{
		if (state == finished) 
		{
			break;
		}
		
		const char ch = pText[i]; 

		switch (state)
		{
			case space:
				if (!isspace(ch))
				{
					state = number;
					start = i;
				}
				break;

			case number:
				if ((ch == 0) || isspace(ch)) 
				{
					state = finished;
					end = i - 1;
				}
				break;

			default:
				assert(false);
				return false;
		}
	}

	if (state == number) 
	{
		end = textLen - 1;
		state = finished;
	}

	if (state != finished) 
	{
		return false;
	}

	if (pStart != NULL)
	{
		*pStart = start;
	}

	if (pEnd != NULL)
	{
		*pEnd = end;
	}

	return true;
}

// Преобразует символ в число
static bool charToNumber(char ch, uint8_t * const pResult, bool digitsOnly)
{
	assert(pResult != NULL);
	if (pResult == NULL)
	{
		return false;
	}

	uint8_t result;

	if ((ch >= '0') && (ch <= '9'))
	{
		result = ch - '0';
	}
	else
	{
		if (digitsOnly)
		{
			return false;
		}

		if ((ch >= 'A') && (ch <= 'F'))
		{
			result = ch - 'A' + 10;
		}
		else if ((ch >= 'a') && (ch <= 'f'))
		{
			result = ch - 'a' + 10;
		}
		else
		{
			return false;
		}
	}

	*pResult = result;
	return true;
}

// Проверяет попадание целого значения в диапазон допустимых значений
static bool isIntegralValueRangeOk(uint64_t valueAbs, size_t resultSize, bool isSigned, bool isNegative)
{
	static const uint64_t maxUnsigned[9] = { [1] = UINT8_MAX,[2] = UINT16_MAX,[4] = UINT32_MAX,[8] = UINT64_MAX };
	static const uint64_t maxSigned[9] = { [1] = INT8_MAX,[2] = INT16_MAX,[4] = INT32_MAX,[8] = INT64_MAX };
	static const uint64_t minSignedAbs[9] = { [1] = -INT8_MIN,[2] = -INT16_MIN,[4] = -INT32_MIN,[8] = -INT64_MIN };

	bool result;

	if (isSigned)
	{
		const uint64_t maxValue = maxSigned[resultSize];
		const uint64_t minValueAbs = minSignedAbs[resultSize];
		result = (maxValue > 0) && (minValueAbs > 0);

		if (isNegative)
		{
			result &= valueAbs <= minValueAbs;
		}
		else
		{
			result &= valueAbs <= maxValue;
		}		
	}
	else
	{
		const uint64_t maxValue = maxUnsigned[resultSize];
		result = maxValue > 0;

		result &= valueAbs <= maxValue;
	}

	return result;
}

static bool splitRegularFloatString(const char * const pText, size_t textLen, char decimalSeparator,
	const char * * const ppIntPart, size_t * const pIntPartLen, const char * * const ppFracPart, size_t * const pFracPartLen)
{
	bool argOk = true;

	argOk &= pText != NULL;
	argOk &= textLen > 0;
	argOk &= ppIntPart != NULL;
	argOk &= pIntPartLen != NULL;
	argOk &= ppFracPart != NULL;
	argOk &= pFracPartLen != NULL;

	if (!argOk)
	{
		return false;
	}

	const char * pPoint; // указатель на разделитель целой и дробной частей в строке

	if (decimalSeparator == 0) // автоматический выбор разделителя
	{
		pPoint = memchr(pText, '.', textLen); // поищем точку
		if (pPoint == NULL) // точки нету
		{
			pPoint = memchr(pText, ',', textLen); // поищем запятую
		}
	}
	else // разделитель задан в явной форме
	{
		pPoint = memchr(pText, decimalSeparator, textLen); // поищем разделитель
	}

	if (pPoint == NULL) // Нет разделителя. Вся строка - целая часть
	{
		// целая часть
		*ppIntPart = pText;
		*pIntPartLen = textLen;

		// дробной нет
		*ppFracPart = NULL;
		*pFracPartLen = 0;
	}
	else if (pPoint == pText)  // Разделитель в начале строки. Вся строка после разделителя - дробная часть
	{
		// целой нет
		*ppIntPart = NULL;
		*pIntPartLen = 0;

		// дробная после разделителя
		*pFracPartLen = textLen - 1;
		if (*pFracPartLen > 0) // если есть что-то, кроме разделителя
		{
			*ppFracPart = &pText[1]; // дробная часть
		}
		else // вся строка - точка
		{
			*ppFracPart = NULL; // дробной тоже нет
		}
	}
	else if (pPoint == &pText[textLen - 1]) // Разделитель в конце строки. Вся строка, кроме последнего символа, - целая часть
	{
		// целая часть
		*ppIntPart = pText;
		*pIntPartLen = textLen - 1;

		// дробной нет
		*ppFracPart = NULL;
		*pFracPartLen = 0;
	}
	else // Разделитель где-то внутри строки
	{
		// целая часть
		*ppIntPart = pText;
		*pIntPartLen = pPoint - pText;

		// дробная часть
		*ppFracPart = pPoint + 1;
		*pFracPartLen = textLen - *pIntPartLen - 1;		
	}

	return true;
}

// Корректировка номера ошибки
static StringToNumResult_t adjustError(StringToNumResult_t err)
{
	// Нужно, когда функция преобразования вызывает другую функцию преобразования.
	// В этом случае надо передавать наверх только ошибки, обусловленные проблемами
	// пользовательских данных. А всякие null-reference и т.п. следует трактовать 
	// как InternalError, т.к. они, очевидно, обусловлены ошибками в вызывающем коде.
	
	switch (err)
	{
	case stnrOk:
		return stnrOk;

	case stnrOverflow:
		return stnrOverflow;

	case stnrUnderflow:
		return stnrUnderflow;

	case stnrSyntaxError:
		return stnrSyntaxError;

	default:
		return stnrInternalError;
	}
}

// Преобразование в число с плавающей точкой строки в обычной форме
static StringToNumResult_t stringToFloatRegular(const char * const pText, size_t textLen, void * const pResult, size_t resultSize, char decimalSeparator)
{
	const char * pIntPart;
	size_t intPartLen;

	const char * pFracPart;
	size_t fracPartLen;

	// определяем границы целой и дробной части
	const bool splitOk = splitRegularFloatString(pText, textLen, decimalSeparator, &pIntPart, &intPartLen, &pFracPart, &fracPartLen);
	if (!splitOk)
	{
		return stnrInternalError;
	}

	if ((pIntPart == NULL) && (pFracPart == NULL)) // нет ни целой, ни дробной части
	{
		return stnrSyntaxError;
	}

	float value = 0.0F;

	if (pIntPart != NULL) // целая часть присутствует
	{
		if (intPartLen == 0)
		{
			return stnrInternalError;
		}

		int64_t intPartValue;
		
		// конвертируем целую часть
		StringToNumResult_t res = tryStringToInt(pIntPart, intPartLen, &intPartValue, sizeof(intPartValue));
		res = adjustError(res);
		
		if (res != stnrOk)
		{
			return res;
		}		

		value = (float)intPartValue; // запоминаем целую часть
		const int64_t tmp = (int64_t)value;

		if (intPartValue != tmp) // что-то пошло не так
		{
			return stnrOverflow; // видимо, переполнение
		}
	}

	if (pFracPart != NULL) // дробная часть присутствует
	{
		if (fracPartLen == 0)
		{
			return stnrInternalError;
		}

		float multiplier = 0.1F; // начнаем с одной десятой

        if (value < 0.0F)
        {
            multiplier *= -1;
        }

		for (size_t i = 0; i < fracPartLen; ++i) // идём по символам строки
		{
			uint8_t tmp;
			// преобразуем текущий символ
			const bool charOk = charToNumber(pFracPart[i], &tmp, true);
			
			if (!charOk) // н.ё.х.
			{
				return stnrSyntaxError;
			}

			value += multiplier * tmp; // накапливаем

			if (i < fracPartLen - 1) // не последний символ
			{
				multiplier *= 0.1F; // обновляем множитель
			}
		}
	}

	memcpy(pResult, &value, resultSize); // выводим результат
	return stnrOk;
}

// Преобразование строки в число с плавающей точкой
StringToNumResult_t tryStringToFloat(const char * const pText, size_t textLen, void * const pResult, size_t resultSize, char decimalSepeartor)
{
	bool argOk = true;

	argOk &= (pText != NULL);
	argOk &= (textLen > 0);
	argOk &= (pResult != NULL);
	argOk &= (resultSize > 0);

	if (!argOk)
	{
		return stnrBadArgument;
	}

	float resultValue;

	argOk &= (resultSize == sizeof(resultValue));

	if (!argOk)
	{
		return stnrUnsupported;
	}

	size_t start; // начало числа в строке
	size_t end; // окончание числа в строке

	// определяем границы числа в строке
	const bool boundsOk = findBounds(pText, textLen, &start, &end);

	if (!boundsOk)
	{
		return stnrSyntaxError;
	}

	const size_t actualLen = end - start + 1;

	// поищем экспоненту
	const char * pExpChar = memchr(&pText[start], 'E', actualLen); // большую
	if (pExpChar == NULL)
	{
		pExpChar = memchr(&pText[start], 'e', actualLen); // и маленькую
	}

	const char * pMantissa;
	size_t mantissaLen;

	const char * pExponent;
	size_t exponentLen;

	if (pExpChar == NULL) // экспоненты нету (число записано в обычной форме)
	{
		// будем считать, что вся строка мантисса
		pMantissa = &pText[start];
		mantissaLen = actualLen;

		// а экспонента как бы 0
		pExponent = NULL;
		exponentLen = 0;
	}
	else if (pExpChar == &pText[start]) // экспонента в начале строки
	{
		return stnrSyntaxError; // нафиг такое
	}
	else // экспонента где-то в середине строки
	{
		// до экспоненты мантисса
		pMantissa = &pText[start];
		mantissaLen = pExpChar - pMantissa;

		// после экспоненты показатель
		pExponent = pExpChar + 1;
		exponentLen = actualLen - mantissaLen - 1;
	}

	// разбираемся с мантиссой
	StringToNumResult_t result = stringToFloatRegular(pMantissa, mantissaLen, &resultValue, sizeof(resultValue), decimalSepeartor);
	result = adjustError(result);
	if (result != stnrOk)
	{
		return result;
	}

	int16_t exponent = 0;

	if (exponentLen > 0) // экспонента присутствует
	{
		// преобразуем показатель
		result = tryStringToInt(pExponent, exponentLen, &exponent, sizeof(exponent));
		result = adjustError(result);
		if (result != stnrOk)
		{
			return result;
		}

		float multiplier;

		if (exponent > 0) // показатель больше нуля
		{
			multiplier = 10.0F; // будем умножать
		}
		else if (exponent < 0) // показатель меньше нуля
		{
			multiplier = 0.1F; // будем делить
			exponent = -exponent; // положительное количество раз
		}
		else // показатель равен нулю
		{
			multiplier = 1.0F; // ничего не будем делать
		}

		if (exponent != 0) // показатель не равен нулю
		{
			for (int i = 0; i < exponent; ++i) // умножаем значение на 10^exponent
			{
				resultValue *= multiplier;
				const bool ok = isfinite(resultValue); // переполнение?

				if (!ok) // если переполнение
				{
					// выходим
					// Ругаться, видимо, нецелесообразно. 
					// Вызывающий код inf может и сам определить, а ноль - он и в Африке ноль.
					break;
				}
			}
		}
	}

	memcpy(pResult, &resultValue, resultSize);
	return result;
}

// Преобразование строки в знаковое целое
StringToNumResult_t tryStringToInt(const char * const pText, size_t textLen, void * const pResult, size_t resultSize)
{
	bool argOk = true;

	argOk &= (pText != NULL);
	argOk &= (textLen > 0);
	argOk &= (pResult != NULL);
	argOk &= (resultSize > 0);
	
	if (!argOk)
	{
		return stnrBadArgument;
	}

	argOk &= ((resultSize == sizeof(int8_t)) || (resultSize == sizeof(int16_t)) || (resultSize == sizeof(int32_t)) || (resultSize == sizeof(int64_t)));
	
	if (!argOk)
	{
		return stnrUnsupported;
	}

	size_t start;
	size_t end;

	// Определяем границы
	const bool boundsOk = findBounds(pText, textLen, &start, &end);

	if (!boundsOk)
	{
		return stnrSyntaxError;
	}

	// Определяем знак
	const bool isNegative = pText[start] == '-';
	if (isNegative)
	{
		++start;
	}

	if (start > end)
	{
		return stnrSyntaxError;
	}

	const size_t actualLen = end - start + 1;	
		
	uint64_t valueAbs;
	// Преобразуем модуль
	const StringToNumResult_t res = tryStringToUint(&pText[start], actualLen, &valueAbs, sizeof(valueAbs), 10);

	if (res != stnrOk)
	{
		return res;
	}

	// Проверяем диапазон
	const bool rangeOk = isIntegralValueRangeOk(valueAbs, resultSize, true, isNegative);
	if (!rangeOk)
	{
		return stnrOverflow;
	}

	// Преобразуем в знаковое
	int64_t signedVal = (int64_t)valueAbs;

	if (isNegative) // если отрицательное
	{
		signedVal = -signedVal; // инвертируем
	}
	
	memcpy(pResult, &signedVal, resultSize);

	return stnrOk;
}

// Преобразование строки в беззнаковое целое
StringToNumResult_t tryStringToUint(const char * const pText, size_t textLen, void * const pResult, size_t resultSize, unsigned	int base)
{
	static const unsigned int maxBase = 16;

	bool argOk = true;

	argOk &= (pText != NULL);
	argOk &= (textLen > 0);
	argOk &= (pResult != NULL);
	argOk &= (resultSize > 0);
	argOk &= (base > 1);
	
	if (!argOk)
	{
		return stnrBadArgument;
	}

	argOk &= ((resultSize == sizeof(uint8_t)) || (resultSize == sizeof(uint16_t)) || (resultSize == sizeof(uint32_t)) || (resultSize == sizeof(uint64_t)));
	argOk &= base <= maxBase;

	if (!argOk)
	{
		return stnrUnsupported;
	}
		
	size_t start;
	size_t end;

	// Определяем границы
	const bool boundsOk = findBounds(pText, textLen, &start, &end);

	if (!boundsOk)
	{
		return stnrSyntaxError;
	}

	const ptrdiff_t iStart = (ptrdiff_t)start;
	const ptrdiff_t iEnd = (ptrdiff_t)end;

	uint64_t sum = 0;
	uint64_t multiplier = 1;

	for (ptrdiff_t i = iEnd; i >= iStart; --i) // проходим с конца
	{
		uint8_t value;
		const char ch = pText[i];

		// преобразуем текущий символ
		const bool charOk = charToNumber(ch, &value, false);
		if ((!charOk) || (value >= base))
		{
			return stnrSyntaxError;
		}		

		uint64_t tmp = multiplier;

		if ((value > 0) && (multiplier * value < tmp)) // проверяем переполнение
		{
			return stnrOverflow;
		}
		
		tmp = sum;
		sum += multiplier * value; // накапливаем

		if (sum < tmp) // снова проверяем переполнение
		{
			return stnrOverflow;
		}
		
		if (i > iStart) // если не последний символ
		{
			tmp = multiplier;
			multiplier *= base; // обновляем множитель

			if (multiplier <= tmp) // и опять проверяем переполнение
			{
				return stnrOverflow;
			}
		}
	}

	// проверяем диапазон
	const bool rangeOk = isIntegralValueRangeOk(sum, resultSize, false, false);

	if (!rangeOk)
	{
		return stnrOverflow;
	}

	memcpy(pResult, &sum, resultSize);
	return stnrOk;
}
