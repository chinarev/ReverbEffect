#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* < ------- first attempt, not useable ------- >
int getDataSound(FILE *file, long offset, int startPoint, int numberFluctuations, int bytePerSample, int *soundArray, int *errorCode)
{
	int i;                      // Итератор
	int j;                      // Итератор
	int *bitBuffer;             // Временный буффер для хранения одного колебания
								// Выделение памяти под временный буффер
	bitBuffer = (int *)malloc(bytePerSample * sizeof(int));

	// Смещение
	fseek(file, offset, startPoint);

	for (i = 0; i < numberFluctuations; ++i)
	{
		//  Побайтовое чтение данных об одном колебании и преобразование в массив данных int
		for (j = 0; j < bytePerSample; ++j)
		{
			bitBuffer[j] = fgetc(file);
			printf("%d\n", bitBuffer[j]);
			system("pause");
		}
		soundArray[i] = byteToInt(bitBuffer, bytePerSample);
		printf("%d\n", soundArray[i]);
	}
}
*/
// Преобразование массива байтов в число
int byteToInt(unsigned char *bytes, int numbytes)
{
	int i;
	int multiplier;
	int result;

	multiplier = 256;
	result = bytes[0];

	for (i = 1; i < numbytes; ++i)
	{
		result += bytes[i] * multiplier;
		multiplier *= 256;
	}

	return result;
}

// Преобразование числа в массив данных
void intToByte(int number, int *result, int numBytes)
{
	int i;
	int multiplier;

	multiplier = 256;
	for (i = 0; i < numBytes; ++i)
	{
		result[i] = number % multiplier;
		number /= multiplier;
	}
}

// Перевод массива байт в массив амплитуд
void byteArrayToAmplitude(unsigned char *byteArray, int *amplitudeArray, int numberFluctuations, int bytePerSample)
{
	int i;
	int j;
	int k;
	int multiplier;

	k = 0;
	for (i = 0; i < numberFluctuations; ++i)
	{
		amplitudeArray[i] = byteArray[k++];
		multiplier = 256;
		for (j = 1; j < bytePerSample; ++j)
		{
			amplitudeArray[i] += byteArray[k++] * multiplier;
			multiplier *= 256;
		}
	}
}

// Перевод массива амплитуд в массив байт
void AmplitudeArrayToByte(unsigned char *byteArray, unsigned int *amplitudeArray, int numberFluctuations, int bytePerSample)
{
	int i;
	int j;
	int k;
	int multiplier;

	k = 0;
	for (i = 0; i < numberFluctuations; ++i)
	{
		multiplier = 256;
		for (j = 0; j < bytePerSample; ++j)
		{
			byteArray[k++] = amplitudeArray[i] % multiplier;
			amplitudeArray[i] /= multiplier;
		}
	}
}

int main(int argc, char* argv[]) // command line
{
	int numChannels;            // Количество каналов
	int sampleRate;             // Частота дискретизации
	int bitPerSample;           // Количество бит в сэмпле
	int bytePerSample;          // Количество байт в сэмпле
	int numberFluctuations;     // Количество колебаний в сэмпле
	int subchunk2Size;          // Количество байт в области данных
	int levels;                 // Количество уровней амплитуды
	unsigned int *soundArray;   // Массив для временного хранения колебаний

	int errorCode;              // Содержит код ошибки
	int multiplier;             // Множитель
	int i;                      // Итератор
	int j;                      // Итератор
	int k;                      // Итератор
	unsigned char *header;      // Массив для хранения заголовка файла
	unsigned char *body;        // Массив для хранения тела исходного файла
	unsigned char *releaseBody; // Массив для хранения тела обработанного файла
	FILE *sound;                // Указатель на файл для чтения сэмпла
	FILE *release;              // Указатель на файл для записи сэмпла

	int *temp;

	// Открываем файл, через параметр командной строки
	
	sound = fopen(argv[1], "rb");;

	// Выделение памяти под заголовок файла
	header = (unsigned char *)malloc(44 * sizeof(unsigned char));

	// Чтение заголовка файла
	fread(header, 1, 44, sound);

	numChannels = byteToInt(header + 22, 2);
	sampleRate = byteToInt(header + 24, 4);
	bitPerSample = byteToInt(header + 34, 2);
	subchunk2Size = byteToInt(header + 40, 4);

	// Вычисление дополнительных параметров на основе основных полученных данных
	bytePerSample = bitPerSample / 8;
	numberFluctuations = subchunk2Size / bytePerSample;

	// Вывод информации о сэмпле
	printf("numChannels = %d\n", numChannels);
	printf("sampleRate = %d\n", sampleRate);
	printf("bitPerSample = %d\n", bitPerSample);
	printf("subchunk2Size = %d\n", subchunk2Size);
	printf("bytePerSample = %d\n", bytePerSample);
	printf("numberFluctuations = %d\n", numberFluctuations);

	// Выделение памяти под тело файла
	body = (unsigned char *)malloc(subchunk2Size * sizeof(unsigned char));

	// Чтение тела файла
	fread(body, 1, subchunk2Size, sound);

	// Выделение памяти под массив амплитуд
	soundArray = (unsigned int *)malloc(numberFluctuations * sizeof(unsigned int));

	// Перевод массива байт в массив амплитуд
	byteArrayToAmplitude(body, soundArray, numberFluctuations, bytePerSample);

	// Очищение памяти, занимаемой массивом байт исходного файла
	free(body);

	// Обработка массива амплитуд
	for (i = 0; i < numberFluctuations - 10; ++i)
	{
		/*
		Александр Павлович, 

		В этом цикле должна происходить непосредственно работа со звуком, т.е. наложение нужных эффектов и т.п.
		Я пробовал различные способы, но они в 90% случаев просто искажали звуковой файл различными скрипами, храпами и ультразвуком.
		Ниже единственная строка, которая хоть немного накладывает нечто похожее на реверберацию и в то же время не убивает всю дорожку.
		*/
		soundArray[i] += soundArray[i + 10];
	}

	// Выделение памяти под массив байт результирующего файла
	releaseBody = (unsigned char *)malloc(subchunk2Size * sizeof(unsigned char) );

	// Перевод массива амплитуд в массив байт
	AmplitudeArrayToByte(releaseBody, soundArray, numberFluctuations, bytePerSample);

	release = fopen("release.wav", "wb");
	fwrite(header, 1, 44, release);
	fwrite(releaseBody, 1, subchunk2Size, release);


	return 0;
}









