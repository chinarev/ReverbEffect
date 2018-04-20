#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* < ------- first attempt, not useable ------- >
int getDataSound(FILE *file, long offset, int startPoint, int numberFluctuations, int bytePerSample, int *soundArray, int *errorCode)
{
	int i;                      // ��������
	int j;                      // ��������
	int *bitBuffer;             // ��������� ������ ��� �������� ������ ���������

								// ��������� ������ ��� ��������� ������
	bitBuffer = (int *)malloc(bytePerSample * sizeof(int));

	// ��������
	fseek(file, offset, startPoint);

	for (i = 0; i < numberFluctuations; ++i)
	{
		// ���������� ������ ������ �� ����� ��������� � �������������� � ������ ������ int
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
// �������������� ������� ������ � �����
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

// �������������� ����� � ������ ������
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

// ������� ������� ���� � ������ ��������
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

// ������� ������� �������� � ������ ����
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

int main(int argc, char* argv[]) // () - ��� ������ � ��������� ������
{
	int numChannels;            // ���������� �������
	int sampleRate;             // ������� �������������
	int bitPerSample;           // ���������� ��� � ������
	int bytePerSample;          // ���������� ���� � ������
	int numberFluctuations;     // ���������� ��������� � ������
	int subchunk2Size;          // ���������� ���� � ������� ������
	int levels;                 // ���������� ������� ���������
	unsigned int *soundArray;   // ������ ��� ���������� �������� ���������

	int errorCode;              // �������� ��� ������
	int multiplier;             // ���������
	int i;                      // ��������
	int j;                      // ��������
	int k;                      // ��������
	unsigned char *header;      // ������ ��� �������� ��������� �����
	unsigned char *body;        // ������ ��� �������� ���� ��������� �����
	unsigned char *releaseBody; // ������ ��� �������� ���� ������������� �����
	FILE *sound;                // ��������� �� ���� ��� ������ ������
	FILE *release;              // ��������� �� ���� ��� ������ ������

	int *temp;

	// ��������� ����, ����� �������� ��������� ������
	
	sound = fopen(argv[1], "rb");;

	// ��������� ������ ��� ��������� �����
	header = (unsigned char *)malloc(44 * sizeof(unsigned char));

	// ������ ��������� �����
	fread(header, 1, 44, sound);

	numChannels = byteToInt(header + 22, 2);
	sampleRate = byteToInt(header + 24, 4);
	bitPerSample = byteToInt(header + 34, 2);
	subchunk2Size = byteToInt(header + 40, 4);

	// ���������� �������������� ���������� �� ������ �������� ���������� ������
	bytePerSample = bitPerSample / 8;
	numberFluctuations = subchunk2Size / bytePerSample;

	// ����� ���������� � ������
	printf("numChannels = %d\n", numChannels);
	printf("sampleRate = %d\n", sampleRate);
	printf("bitPerSample = %d\n", bitPerSample);
	printf("subchunk2Size = %d\n", subchunk2Size);
	printf("bytePerSample = %d\n", bytePerSample);
	printf("numberFluctuations = %d\n", numberFluctuations);

	// ��������� ������ ��� ���� �����
	body = (unsigned char *)malloc(subchunk2Size * sizeof(unsigned char));

	// ������ ���� �����
	fread(body, 1, subchunk2Size, sound);

	// ��������� ������ ��� ������ ��������
	soundArray = (unsigned int *)malloc(numberFluctuations * sizeof(unsigned int));

	// ������� ������� ���� � ������ ��������
	byteArrayToAmplitude(body, soundArray, numberFluctuations, bytePerSample);

	// �������� ������, ���������� �������� ���� ��������� �����
	free(body);

	// ��������� ������� ��������
	for (i = 0; i < numberFluctuations - 10; ++i)
	{
		/*
		��������� ��������, 

		� ���� ����� ������ ����������� ��������������� ������ �� ������, �.�. ��������� ������ �������� � �.�.
		� �������� ��������� �������, �� ��� � 90% ������� ������ �������� �������� ���� ���������� ��������, ������� � ������������.
		���� ������������ ������, ������� ���� ������� ����������� ����� ������� �� ������������ � � �� �� ����� �� ������� ��� �������.
		*/
		soundArray[i] += soundArray[i + 10];
	}

	// ��������� ������ ��� ������ ���� ��������������� �����
	releaseBody = (unsigned char *)malloc(subchunk2Size * sizeof(unsigned char) );

	// ������� ������� �������� � ������ ����
	AmplitudeArrayToByte(releaseBody, soundArray, numberFluctuations, bytePerSample);

	release = fopen("release.wav", "wb");
	fwrite(header, 1, 44, release);
	fwrite(releaseBody, 1, subchunk2Size, release);


	return 0;
}









