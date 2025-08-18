#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

static FILE *stream = NULL;

void set_stream(FILE *f)
{
	stream = f;
}

FILE *get_stream()
{
	return stream ? stream : stdout;
}


void extract_id_from_header(char *dst, const char *src)
{
	while (*src && isspace((unsigned char)*src)) src++;
	if (isdigit((unsigned char)*src)) {
		while (isdigit((unsigned char)*src)) {
			*dst++ = *src++;
		}
		*dst = '\0';
	} else {
		*dst = '\0';
	}
}

void tag(const char *name, const char *content_fmt, ...)
{
	FILE *f = get_stream();
	va_list args;
	fprintf(f, "<%s>", name);
	va_start(args, content_fmt);
	vfprintf(f, content_fmt, args);
	va_end(args);
	fprintf(f, "</%s>\n", name);
}

void tag_open(const char *name, const char *attrs)
{
	FILE *f = get_stream();
	if (attrs)
		fprintf(f, "<%s %s>\n", name, attrs);
	else
		fprintf(f, "<%s>\n", name);
}

void tag_close(const char *name)
{
	FILE *f = get_stream();
	fprintf(f, "</%s>\n", name);
}

void tag_inline(const char *name, const char *attrs, const char *text)
{
	FILE *f = get_stream();
	if (attrs)
		fprintf(f, "<%s %s>%s</%s>", name, attrs, text, name);
	else
		fprintf(f, "<%s>%s</%s>", name, text, name);
}

void tag_inline_open(const char *name, const char *attrs, const char *text)
{
	FILE *f = get_stream();
	if (attrs)
		fprintf(f, "<%s %s>%s", name, attrs, text);
	else
		fprintf(f, "<%s>%s", name, text);
}

void begin_html()
{
	FILE *f = get_stream();
	tag_open("html", "lang=\"ru\"");
}

void end_html()
{
	tag_close("html");
}

void head(const char *title, const char *styles_file)
{
	tag_open("head", NULL);
	FILE *f = get_stream();
	fprintf(f, "<meta charset=\"UTF-8\">\n");
	fprintf(f, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
	tag("title", "%s", title);

	if (styles_file) cmtag_css_link(styles_file);

	tag_close("head");
}

void begin_body()
{
	tag_open("body", NULL);
}

void end_body()
{
	tag_close("body");
}

// custom tag
void cmtag_blog_post(const char *date, const char *content)
{
	tag_open("div", "class=\"post-item\"");
	tag("time", "[%s]", date);
	tag("span", "%s", content);
	tag_close("div");
}

void cmtag_css_link(const char *filename)
{
	FILE *f = get_stream();
	fprintf(f, "<link rel=\"stylesheet\" href=\"%s\">\n", filename);
}

void cmtag_a(const char *href, const char *text)
{
	FILE *f = get_stream();
	fprintf(f, "<a href=\"%s\">%s</a>", href, text);
}

void cmtag_img(const char *url, char *alt)
{
	FILE *f = get_stream();
	fprintf(f, "<img src=\"%s\" alt=\"%s\">", url, alt);
}

void cmtag_h1(char *text)
{
	char id[256];
	extract_id_from_header(id, text);

	char attr[256];
	if (id[0] != '\0')
		sprintf(attr, "id=\"%s\"", id);
	else
		attr[0] = '\0';

	tag_inline("h1", attr[0] ? attr : NULL, text);
}

int genHTML(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (!f) {
		perror("Error opening input file");
		return -1;
	}

	char buf[1024];
	unsigned char in_list = 0;
	unsigned char in_code = 0;

	while (fgets(buf, sizeof(buf), f)) {
		FILE *f = get_stream();
		buf[strcspn(buf, "\r\n")] = 0;

		if (strcmp(buf, "```") == 0) {
			if (!in_code) {
				fputs("<pre><code>", f);
				in_code = 1;
			} else {
				fputs("</code></pre>", f);
				in_code = 0;
			}
			continue;
		} else if (in_code) {
			for (char *p = buf; *p; p++) {
				switch (*p) {
				case '&': fputs("&amp;", f); break;
				case '<': fputs("&lt;", f); break;
				case '>': fputs("&gt;", f); break;
				default: fputc(*p, f);
				}
			}
			fputc('\n', f);
			continue;
		} else if (strncmp(buf, "$ ", 2) == 0) {
			char t[256];
			sprintf(t, "%s", buf + 2);
			cmtag_h1(t);
		} else if (strncmp(buf, "$$ ", 3) == 0) {
			tag("h2", "%s", buf + 3);
		} else if (strncmp(buf, "$$$ ", 4) == 0) {
			tag("h3", "%s", buf + 4);
		} else if (strncmp(buf, "- ", 2) == 0) {
			if (!in_list) {
				tag_open("ul", NULL);
				in_list = 1;
			}
			tag("li", "%s", buf + 2);
		} else if (strncmp(buf, "-- ", 3) == 0) {
			tag("ul", "<li>%s</li>", buf + 3);

		} else if (strncmp(buf, "![", 2) == 0) {
			// [alt](url)
			char alt[256], url[256];
			if (sscanf(buf, "![%255[^]]](%255[^)])", alt, url) == 2) {
				cmtag_img(url, alt);
			}
		} else if (buf[0] == '[') {
			// [text](url)
			char text[256], url[256];
			if (sscanf(buf, "[%255[^]]](%255[^)])", text, url) == 2) {
				cmtag_a(url, text);
			}
		} else if (strncmp(buf, "__ ", 3) == 0) {
			tag("p", "%s", buf + 3);
		} else {
			if (in_list) {
				tag_close("ul");
				in_list = 0;
			}
			tag("p", "%s", buf);
		}
	}
	fclose(f);
	return 0;
}

int main(int argc, char *argv[])
{
	const char *input_file = NULL;
	const char *output_file = NULL;
	const char *styles_file = "style.css";

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
			input_file = argv[++i];
		} else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
			output_file = argv[++i];
		} else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
			styles_file = argv[++i];
		} else {
			fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
			return 1;
		}
	}

	if (!input_file) {
		fprintf(stderr, "Usage: %s -i <input_file> [-o <output_file>] [-s <style_file>]\n", argv[0]);
		return 1;
	}

	FILE *out = stdout;
	if (output_file) {
		out = fopen(output_file, "w");
		if (!out) {
			perror("Error opening output file");
			return 1;
		}
	}

	set_stream(out);

	begin_html();
	head("URANCORE", styles_file);
	begin_body();

	genHTML(input_file);

	end_body();
	end_html();

	if (output_file) {
		fclose(out);
	}

	return 0;
}
