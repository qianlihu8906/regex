#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct state;

struct edge{
#define EPLISON		-1
	int c;
	struct state *next;
};

struct state{
	char name[64];
	struct edge *e1;
	struct edge *e2;
};

struct frag{
	struct state *start;
	struct state *end;
};


static struct state *state_new()
{
	static int i;
	struct state *s = malloc(sizeof(*s));
	assert(s != NULL);
	memset(s,0,sizeof(*s));

	char name[64];
	sprintf(name,"%d",i);
	strncpy(s->name,name,sizeof(s->name));
	i++;
	return s;
}

static struct edge *edge_new(int c,struct state *next)
{
	struct edge *e = malloc(sizeof(*e));
	assert(e != NULL);
	memset(e,0,sizeof(*e));

	e->c = c;
	e->next = next;

	return e;
}

static struct frag *frag_new()
{
	struct frag *f = malloc(sizeof(*f));
	assert(f != NULL);
	memset(f,0,sizeof(*f));

	struct state *start = state_new();
	struct state *end = state_new();


	f->start = start;
	f->end = end;

	return f;
}

static int state_add_edge(struct state *s,struct edge *e1,struct edge *e2)
{
	s->e1 = e1;
	s->e2 = e2;

	return 0;
}


static struct frag *character(int c)
{
	struct frag *f = frag_new();

	struct edge *edge = edge_new(c,f->end);
	state_add_edge(f->start,edge,NULL);

	return f;
}

static struct frag *concatenation(struct frag *f1,struct frag *f2)
{
	struct frag *f = frag_new();

	struct edge *edge = edge_new(EPLISON,f1->start);	
	state_add_edge(f->start,edge,NULL);
	edge = edge_new(EPLISON,f->end);
	state_add_edge(f2->end,edge,NULL);	

	edge = edge_new(EPLISON,f2->start);
	state_add_edge(f1->end,edge,NULL);	
	return f;
}

static struct frag *split(struct frag *f1,struct frag *f2)
{
	struct frag *f = frag_new();
	struct edge *edge1 = edge_new(EPLISON,f1->start);
	struct edge *edge2 = edge_new(EPLISON,f2->start);
	state_add_edge(f->start,edge1,edge2);

	struct edge *edge = edge_new(EPLISON,f->end);
	state_add_edge(f1->end,edge,NULL);
	state_add_edge(f2->end,edge,NULL);

	return f;
}

static void state_print(struct state *s)
{
	if(s == NULL)
		return;
	printf("state:%s\n",s->name);
	if(s->e1 != NULL){
		if(s->e1->c == EPLISON)
			printf("edge1:eplision\n");	
		else
			printf("edge1:%c\n",s->e1->c);
		state_print(s->e1->next);
	}
	printf("----------\n");
	if(s->e2 != NULL){
		if(s->e2->c == EPLISON)
			printf("edge2:eplision\n");	
		else
			printf("edge2:%c\n",s->e1->c);
		state_print(s->e2->next);
	}
}

int main()
{
	struct frag *f = character('c');
	struct frag *f1 = character('b');
	f = split(f,f1);

	state_print(f->start);
}


