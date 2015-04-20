#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "markdown.h"

#define INDENT "    "
#define NEWLINE_INDENT "\n" INDENT

static char TestStr[] = 
"You&#39;re using `concat` in place of `format`. Also, your query is wrong. You&#39;re using `+` instead of `AND`, there&#39;s a single quote missing after first `&#39;%{0}%`, and `description + &#39;%{0}%&#39;` doesn&#39;t make sense. Try this: ((DataTable)gridTestCodes.DataSource).DefaultView.RowFilter = string.Format(&quot;name like &#39;%{0}%&#39; AND description like &#39;%{0}%&#39;&quot;, txtSearch.Text.Trim().Replace(&quot;&#39;&quot;, &quot;&#39;&#39;&quot;)); Or try just querying on name first and then adding in description once this works (also should the `.Replace` be removing the quotes all together?): ((DataTable)gridTestCodes.DataSource).DefaultView.RowFilter = string.Format(&quot;name like &#39;%{0}%&#39;&quot;, txtSearch.Text.Trim().Replace(&quot;&#39;&quot;, string.empty)); **EDIT:** If you want to enter name then description seperated by comma try: var name = txtSearch.Text.Split(&#39;,&#39;)[0].Trim().Replace(&quot;&#39;&quot;, string.empty); var description = txtSearch.Text.Split(&#39;,&#39;)[1].Trim().Replace(&quot;&#39;&quot;, string.empty); ((DataTable) gridTestCodes.DataSource).DefaultView.RowFilter = string.Format(&quot;name like &#39;%{0}%&#39; AND description like &#39;%{1}%&#39;&quot;, name, description); Note that the above code will throw an exception if the input string **does not** contain a comma. So you may want to add a check for that.";

char translate_entity(char* s) {
	if(strcmp(s, "quot") == 0) {
      return '\'';
    } else if(strcmp(s, "gt") == 0) {
      return '>';
    } else if(s[0] == '#') {
      return (char)atoi(s + 1);
    } else {
		return 0;
	}
}

char get_entity(char* str, int pos, int size) {
	int my_pos = pos;
	my_pos ++;
	for(; my_pos < size && my_pos < pos + 20; my_pos++) {
		if(str[my_pos] == ';') {
			str[my_pos] = '\0';
			char* ent = str + pos + 1;
			return translate_entity(ent);
		}
	}
	return 0;
}

void
term_entity(struct buf *ob, struct buf* text, void* opaque) {
	char ent = get_entity(text->data, 0, text->size);
	if(ent != 0)
		bufputc(ob, ent);
	else
		bufput(ob, text->data, text->size);
}

static void
term_text_escape(struct buf *ob, char *src, size_t size) {
	// this should actually escape terminal escape character
	int i = 0;
	while(i < size) {
		char ch = src[i];
		if(ch == '\n') BUFPUTSL(ob, NEWLINE_INDENT);
		else bufputc(ob, ch);
		i++;
	}
}

static void
term_normal_text(struct buf *ob, struct buf *text, void *opaque) {
	if (text) term_text_escape(ob, text->data, text->size);
}


/* renderer structure */
struct mkd_renderer to_man = {
	/* document-level callbacks */
	NULL,
	NULL,

	/* block-level callbacks */
	NULL, //term_blockcode,
   NULL, //term_blockquote,
	NULL, //NULL,
	NULL, //term_header,
   NULL, //term_hrule,
	NULL, //term_list,
	NULL, //term_listitem,
	NULL, //term_paragraph,
	NULL,
	NULL,
	NULL,

	/* span-level callbacks */
	NULL,
	NULL, //term_codespan,
	NULL, //term_double_emphasis,
	NULL, //term_emphasis,
	NULL, //NULL,
	NULL, //term_linebreak,
	NULL,
	NULL,
	NULL,

	/* low-level callbacks */
	term_entity,
	term_normal_text,

	/* renderer data */
	64,
	"*_",
	NULL
};

int main(void) {
	struct buf *ib, *ob;
   size_t ret;

   puts("starting");

   printf("Test len %d: [%s]\n", strlen(TestStr), TestStr);

	/* performing markdown to man */
   ib = bufnew(strlen(TestStr) + 1);
	ob = bufnew(100);
	markdown(ob, ib, &to_man);

   // fill ib
   strcpy(ib->data, TestStr);

	/* writing the result to stdout */
   printf("%s", INDENT);
	ret = fwrite(ob->data, 1, ob->size, stdout);
	if (ret < ob->size)
		fprintf(stderr, "Warning: only %zu output byte written, "
				"out of %zu\n",
				ret,
				ob->size);

	/* cleanup */
	bufrelease(ib);
	bufrelease(ob);


   return 0;
}

