#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "parser.h"

static FILE *stream = NULL;
static char *styles_file = "style.css";

void set_stream(FILE *f)
{
	stream = f;
}

FILE *get_stream()
{
	return stream ? stream : stdout;
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
	fprintf(f, "\xEF\xBB\xBF"); // UTF-8 BOM
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

int genHTML(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (!f) {
		perror("Error opening input file");
		return -1;
	}

	char buf[1024];
	int in_list = 0;

	while (fgets(buf, sizeof(buf), f)) {
		buf[strcspn(buf, "\r\n")] = 0;

		if (strncmp(buf, "$ ", 2) == 0) {
			tag("h1", "%s", buf + 2);
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

		} else if (buf[0] == '[') {
			// [text](url)
			char text[256], url[256];
			if (sscanf(buf, "[%[^]]](%[^)])", text, url) == 2) {
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

int main()
{
	set_stream(stdout);

	begin_html();
	head("URANCORE", styles_file);
	begin_body();

	genHTML("index");

	end_body();
	end_html();

	return 0;
}
