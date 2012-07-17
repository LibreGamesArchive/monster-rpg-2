#include <cstdio>

const int BORDER = 0;

int main(void)
{
	int x, y;
	int count = 0;

	printf("\t");
	for (y = 0; y+16+BORDER <= 2048; y += 16+BORDER) {
		for (x = 0; x+16+BORDER <= 512; x += 16+BORDER) {
			printf("%4d,", x+BORDER);
			count++;
			if (count == 10) {
				printf("\n\t");
				count = 0;
			}
			else
				printf(" ");
		}
	}

	printf("\n\n\n\n\t");
	for (x = 0; x+16+BORDER <= 2048; x += 16+BORDER) {
		for (y = 0; y+16+BORDER <= 512; y += 16+BORDER) {
			printf("%4d,", x+BORDER);
			count++;
			if (count == 10) {
				printf("\n\t");
				count = 0;
			}
			else
				printf(" ");
		}
	}
}

