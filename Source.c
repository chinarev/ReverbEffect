#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* < ------- first attempt, not useable ------- >
int getDataSound(FILE *file, long offset, int startPoint, int numberFluctuations, int bytePerSample, int *soundArray, int *errorCode)
{
	int i;                      // Èòåðàòîð
	int j;                      // Èòåðàòîð
	int *bitBuffer;             // Âðåìåííûé áóôôåð äëÿ õðàíåíèÿ îäíîãî êîëåáàíèÿ

								// Âûäåëåíèå ïàìÿòè ïîä âðåìåííûé áóôôåð
	bitBuffer = (int *)malloc(bytePerSample * sizeof(int));

	// Ñìåùåíèå
	fseek(file, offset, startPoint);

	for (i = 0; i < numberFluctuations; ++i)
	{
		// Ïîáàéòîâîå ÷òåíèå äàííûõ îá îäíîì êîëåáàíèè è ïðåîáðàçîâàíèå â ìàññèâ äàííûõ int
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

// Ïðåîáðàçîâàíèå ÷èñëà â ìàññèâ äàííûõ
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

// Ïåðåâîä ìàññèâà áàéò â ìàññèâ àìïëèòóä
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

// Ïåðåâîä ìàññèâà àìïëèòóä â ìàññèâ áàéò
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

int main(int argc, char* argv[]) // () - äëÿ ðàáîòû ñ êîìàíäíîé ñòðîêè
{
	int numChannels;            // Êîëè÷åñòâî êàíàëîâ
	int sampleRate;             // ×àñòîòà äèñêðåòèçàöèè
	int bitPerSample;           // Êîëè÷åñòâî áèò â ñýìïëå
	int bytePerSample;          // Êîëè÷åñòâî áàéò â ñýìïëå
	int numberFluctuations;     // Êîëè÷åñòâî êîëåáàíèé â ñýìïëå
	int subchunk2Size;          // Êîëè÷åñòâî áàéò â îáëàñòè äàííûõ
	int levels;                 // Êîëè÷åñòâî óðîâíåé àìïëèòóäû
	unsigned int *soundArray;   // Ìàññèâ äëÿ âðåìåííîãî õðàíåíèÿ êîëåáàíèé

	int errorCode;              // Ñîäåðæèò êîä îøèáêè
	int multiplier;             // Ìíîæèòåëü
	int i;                      // Èòåðàòîð
	int j;                      // Èòåðàòîð
	int k;                      // Èòåðàòîð
	unsigned char *header;      // Ìàññèâ äëÿ õðàíåíèÿ çàãîëîâêà ôàéëà
	unsigned char *body;        // Ìàññèâ äëÿ õðàíåíèÿ òåëà èñõîäíîãî ôàéëà
	unsigned char *releaseBody; // Ìàññèâ äëÿ õðàíåíèÿ òåëà îáðàáîòàííîãî ôàéëà
	FILE *sound;                // Óêàçàòåëü íà ôàéë äëÿ ÷òåíèÿ ñýìïëà
	FILE *release;              // Óêàçàòåëü íà ôàéë äëÿ çàïèñè ñýìïëà

	int *temp;

	// Îòêðûâàåì ôàéë, ÷åðåç ïàðàìåòð êîìàíäíîé ñòðîêè
	
	sound = fopen(argv[1], "rb");;

	// Âûäåëåíèå ïàìÿòè ïîä çàãîëîâîê ôàéëà
	header = (unsigned char *)malloc(44 * sizeof(unsigned char));

	// ×òåíèå çàãîëîâêà ôàéëà
	fread(header, 1, 44, sound);

	numChannels = byteToInt(header + 22, 2);
	sampleRate = byteToInt(header + 24, 4);
	bitPerSample = byteToInt(header + 34, 2);
	subchunk2Size = byteToInt(header + 40, 4);

	// Âû÷èñëåíèå äîïîëíèòåëüíûõ ïàðàìåòðîâ íà îñíîâå îñíîâíûõ ïîëó÷åííûõ äàííûõ
	bytePerSample = bitPerSample / 8;
	numberFluctuations = subchunk2Size / bytePerSample;

	// Âûâîä èíôîðìàöèè î ñýìïëå
	printf("numChannels = %d\n", numChannels);
	printf("sampleRate = %d\n", sampleRate);
	printf("bitPerSample = %d\n", bitPerSample);
	printf("subchunk2Size = %d\n", subchunk2Size);
	printf("bytePerSample = %d\n", bytePerSample);
	printf("numberFluctuations = %d\n", numberFluctuations);

	// Âûäåëåíèå ïàìÿòè ïîä òåëî ôàéëà
	body = (unsigned char *)malloc(subchunk2Size * sizeof(unsigned char));

	// ×òåíèå òåëà ôàéëà
	fread(body, 1, subchunk2Size, sound);

	// Âûäåëåíèå ïàìÿòè ïîä ìàññèâ àìïëèòóä
	soundArray = (unsigned int *)malloc(numberFluctuations * sizeof(unsigned int));

	// Ïåðåâîä ìàññèâà áàéò â ìàññèâ àìïëèòóä
	byteArrayToAmplitude(body, soundArray, numberFluctuations, bytePerSample);

	// Î÷èùåíèå ïàìÿòè, çàíèìàåìîé ìàññèâîì áàéò èñõîäíîãî ôàéëà
	free(body);

	// Îáðàáîòêà ìàññèâà àìïëèòóä
	for (i = 0; i < numberFluctuations - 10; ++i)
	{
		/*
		Àëåêñàíäð Ïàâëîâè÷, 

		Â ýòîì öèêëå äîëæíà ïðîèñõîäèòü íåïîñðåäñòâåííî ðàáîòà ñî çâóêîì, ò.å. íàëîæåíèå íóæíûõ ýôôåêòîâ è ò.ï.
		ß ïðîáîâàë ðàçëè÷íûå ñïîñîáû, íî îíè â 90% ñëó÷àåâ ïðîñòî èñêàæàëè çâóêîâîé ôàéë ðàçëè÷íûìè ñêðèïàìè, õðàïàìè è óëüòðàçâóêîì.
		Íèæå åäèíñòâåííàÿ ñòðîêà, êîòîðàÿ õîòü íåìíîãî íàêëàäûâàåò íå÷òî ïîõîæåå íà ðåâåðáåðàöèþ è â òî æå âðåìÿ íå óáèâàåò âñþ äîðîæêó.
		*/
		soundArray[i] += soundArray[i + 10];
	}

	// Âûäåëåíèå ïàìÿòè ïîä ìàññèâ áàéò ðåçóëüòèðóþùåãî ôàéëà
	releaseBody = (unsigned char *)malloc(subchunk2Size * sizeof(unsigned char) );

	// Ïåðåâîä ìàññèâà àìïëèòóä â ìàññèâ áàéò
	AmplitudeArrayToByte(releaseBody, soundArray, numberFluctuations, bytePerSample);

	release = fopen("release.wav", "wb");
	fwrite(header, 1, 44, release);
	fwrite(releaseBody, 1, subchunk2Size, release);


	return 0;
}









