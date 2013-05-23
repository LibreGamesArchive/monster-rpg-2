#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int value(char c)
{
	if (isdigit(c))
		return c - '0';
	else
		return c - 'A' + 10;
}

static void save_url(FILE *f, const char *buf)
{
	int half = strlen(buf)/2;

	for (int i = 0; i < half; i++) {
		char b1 = *buf;
		buf++;
		char b2 = *buf;
		buf++;
		int high = value(b1);
		int low = value(b2);
		int result = (high << 4) + low;
		fputc(result, f);
	}
}

int main(int argc, char **argv)
{
	FILE *in, *out;
	
	in = fopen(argv[1], "r");
	out = fopen(argv[2], "wb");

	char buf[10000];
	fgets(buf, 10000, in);
	buf[strlen(buf)-1] = 0; // chop newline
	printf("%d\n", buf[strlen(buf)-1]);
	save_url(out, buf);

	fclose(in);
	fclose(out);

	return 0;
}
