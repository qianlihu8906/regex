#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

struct edge{
#define EPSILON   -1
	int c;
	struct state *to;
	struct edge *next;
};

struct state{
	int s;
	struct edge *next;
};

struct State{
	int count;
	struct state state[256];
};

static struct State S;

static struct state *state_new()
{
	struct state *s = &S.state[S.count];
	s->next = NULL;
	s->s = S.count;
	S.count++;
	return s;
}

static void edge_add(int c,struct state *from,struct state *to)
{
	struct edge *e = malloc(sizeof(*e));
	e->c = c;
	e->to = to;

	if(from->next == NULL){
		from->next = e;
	}else{
		e->next = from->next;
		from->next = e;
	}
}

struct nfa{
	struct state *start;
	struct state *end;
};


static struct nfa *nfa_character(int c)
{
	struct nfa *nfa = malloc(sizeof(*nfa));
	nfa->start = state_new();
	nfa->end = state_new();

	edge_add(c,nfa->start,nfa->end);

	return nfa;
}


static struct nfa *nfa_concat(struct nfa *nfa1,struct nfa *nfa2)
{
	struct nfa *nfa = malloc(sizeof(*nfa));

	nfa->start = nfa1->start;
	nfa->end = nfa2->end;
	
	edge_add(EPSILON,nfa1->end,nfa2->start);

	return nfa;
}

static struct nfa *nfa_split(struct nfa *nfa1,struct nfa *nfa2)
{
	struct nfa *nfa = malloc(sizeof(*nfa));

	nfa->start = state_new();
	edge_add(EPSILON,nfa->start,nfa1->start);
	edge_add(EPSILON,nfa->start,nfa2->start);

	nfa->end = state_new();
	edge_add(EPSILON,nfa1->end,nfa->end);
	edge_add(EPSILON,nfa2->end,nfa->end);

	return nfa;
}

static struct nfa *nfa_kleene(struct nfa *nfa1)
{
	struct nfa *nfa = malloc(sizeof(*nfa));

	nfa->start = state_new();
	nfa->end = state_new();

	edge_add(EPSILON,nfa->start,nfa->end);
	edge_add(EPSILON,nfa->start,nfa1->start);
	edge_add(EPSILON,nfa1->end,nfa1->start);
	edge_add(EPSILON,nfa1->end,nfa->end);

	return nfa;
}

static void edge_print(struct state *s)
{
	struct edge *e;
	for(e=s->next;e;e=e->next){
		printf("edge to %d by %c ",e->to->s,e->c); //TODO eplison print ?
	}
}

static void state_print()
{
	int i;
	for(i=0;i<S.count;i++){
		printf("state:%d ",i);
		edge_print(&S.state[i]);
		printf("\n");
	}
}

static struct nfa *nfa_construct()
{
	struct nfa *a = nfa_character('a');
	struct nfa *b = nfa_character('b');
	struct nfa *ab = nfa_concat(a,b);
	struct nfa *ab_kleene = nfa_kleene(ab);

	return ab_kleene;

}

struct queue{
	int len;
	int states[256];
};

static struct queue *queue_create()
{
	struct queue *q = malloc(sizeof(*q));
	q->len = 0;

	return q;
}

static int queue_empty(struct queue *q)
{
	return q->len == 0;
}

static void queue_add(struct queue *q,int state)
{
	q->states[q->len++] = state;
}

static int queue_exist(struct queue *q,int state)
{
	int i;
	for(i=0;i<q->len;i++){
		if(q->states[i] == state)
			return 1;
	}
	return 0;
}

static void queue_debug(struct queue *q)
{
	int i;
	printf("queue: ");
	for(i=0;i<q->len;i++){
		printf("%d ",q->states[i]);
	}
	printf("\n");
}

static void _eps_clourse(struct queue *q,int state)
{
	queue_add(q,state);
	struct state *s = &S.state[state];
	struct edge *e;
	for(e=s->next;e;e=e->next){
		if(e->c == EPSILON)
			if(!queue_exist(q,e->to->s))
				_eps_clourse(q,e->to->s);
	}
}

static struct queue *eps_clourse(int state)
{
	struct queue *q = queue_create();
	if(state < 0)
		return q;
	_eps_clourse(q,state);
	return q;
}

static int move(struct queue *q,int c)
{
	int i;
	for(i=0;i<q->len;i++){
		struct edge *e;
		int state = q->states[i];
		for(e=S.state[state].next;e;e=e->next){
			if(e->c == c)
				return e->to->s;
		}
	}
	return -1;
}

static void match(struct nfa *nfa,const char *p)
{
	struct queue *q = eps_clourse(nfa->start->s);

	for(;*p;p++){
		if(queue_empty(q))
			break;
		queue_debug(q);
		int state = move(q,*p);
		free(q);
		q = eps_clourse(state);
	}
	printf("step finish\n");
	queue_debug(q);
	if(queue_exist(q,nfa->end->s))
		printf("match\n");
	else
		printf("not match\n");
	free(q);
}

int main()
{
	struct nfa *nfa = nfa_construct();
	state_print();
	match(nfa,"ababab");
	printf("%d,%d\n",nfa->start->s,nfa->end->s);
}
