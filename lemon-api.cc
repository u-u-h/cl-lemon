#include <lemon/concepts/digraph.h>

#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/static_graph.h>
#include <lemon/preflow.h>

#include "lemon-api.h"
#include <cstdlib>
#include <cassert>

// For now we are using SmartDigraph throughout.
struct lmn_digraph {
  lemon::SmartDigraph* g;
};

struct lmn_node {
  // this seems awkward, but Node& is failing, and Node* is overhead.
  lemon::SmartDigraph::Node n;
};

struct lmn_arc {
  lemon::SmartDigraph::Arc e;
};
  // struct lmn_node_it;
  // struct lmn_node_map;
  // struct lmn_node_it;

struct lmn_arc_map {
  enum lmn_map_type t;
  union {
    lemon::SmartDigraph::ArcMap<int> *imap;
    lemon::SmartDigraph::ArcMap<unsigned int> *uimap;
    lemon::SmartDigraph::ArcMap<long> *lmap;
    lemon::SmartDigraph::ArcMap<unsigned long> *ulmap;
    lemon::SmartDigraph::ArcMap<float> *fmap;
    lemon::SmartDigraph::ArcMap<double> *dmap;
  } u;
};

struct lmn_flow_computation {
  lemon::SmartDigraph *g; // should be a shared ptr
  lemon::Preflow<lemon::SmartDigraph> *preflow;
  lemon::SmartDigraph::ArcMap<int> *orig_capacities;
  // preallocated and reused when killing some arcs
  lemon::SmartDigraph::ArcMap<int> *tmp_capacities;
  // last value
  double val;
};

/* ctors */
static struct lmn_digraph*
new_lmn_digraph(lemon::SmartDigraph *g) {
  struct lmn_digraph *res
      = (struct lmn_digraph*)malloc(sizeof(struct lmn_digraph));
  if(res) {
    res->g=g;
  }
  return res;
}

static struct lmn_node*
new_lmn_node(const lemon::SmartDigraph::Node n) {
  struct lmn_node *res = (struct lmn_node*)malloc(sizeof(struct lmn_node));
  if(res) {
    res->n=n;
  }
  return res;
}

static struct lmn_arc*
new_lmn_arc(const lemon::SmartDigraph::Arc e) {
  struct lmn_arc *res = (struct lmn_arc*)malloc(sizeof(struct lmn_arc));
  if(res) {
    res->e=e;
  }
  return res;
}

struct lmn_arc_map*
lmn_digraph_new_arc_map(struct lmn_digraph *g,
                        enum lmn_map_type t)
{
  assert(g!=NULL);
  struct lmn_arc_map *res
      = (struct lmn_arc_map*)malloc(sizeof(struct lmn_arc_map));
  if(res!=NULL) {
    res->t=t;
    switch(t) {
      case LMN_MAP_INT:
        res->u.imap = new lemon::SmartDigraph::ArcMap<int>(*(g->g));
        break;
      case LMN_MAP_UINT:
        res->u.uimap = new lemon::SmartDigraph::ArcMap<unsigned int>(*(g->g));
        break;
      case LMN_MAP_LONG:
        res->u.lmap = new lemon::SmartDigraph::ArcMap<long>(*(g->g));
        break;
      case LMN_MAP_ULONG:
        res->u.ulmap = new lemon::SmartDigraph::ArcMap<unsigned long>(*(g->g));
        break;
      case  LMN_MAP_FLOAT:
        res->u.fmap = new lemon::SmartDigraph::ArcMap<float>(*(g->g));
        break;
      case LMN_MAP_DOUBLE:
        res->u.dmap = new lemon::SmartDigraph::ArcMap<double>(*(g->g));
        break;
      default:
        std::cerr << "Undefined map type: " << t <<std::endl;
        assert(0); break;
    }

  }
  return res;
}


/* dtors */
void free_lmn_digraph(struct lmn_digraph* g)
{
  assert(g->g!=nullptr);
  assert(g!=NULL);
  delete g->g;
  free(g);
}
void free_lmn_node(struct lmn_node* n)
{
  assert(n!=NULL);
  free(n);
}

  // void free_lmn_node_it(struct lmn_node_it* ni);
  // void free_lmn_node_map(struct lmn_node_map* nm);
void free_lmn_arc(struct lmn_arc* e)
{
  assert(e!=NULL);
  free(e);
}
  // void free_lmn_edge_it(struct lmn_edge_it* ei);

void free_lmn_arc_map(struct lmn_arc_map* am)
{
  assert(am!=NULL);
  free(am);
  return;
}

void
lmn_arc_map_set_val(struct lmn_arc_map *am,
                    struct lmn_arc *a,
                    union lmn_map_val v)
{
  assert(am!=NULL);
  assert(a!=NULL);
  switch (am->t) {
    case LMN_MAP_INT:
      (*am->u.imap)[a->e] = v.i; break;
    case LMN_MAP_UINT:
      (*am->u.uimap)[a->e] = v.ui; break;
    case LMN_MAP_LONG:
      (*am->u.lmap)[a->e] = v.l; break;
    case LMN_MAP_ULONG:
      (*am->u.ulmap)[a->e] = v.ul; break;
    case    LMN_MAP_FLOAT:
      (*am->u.fmap)[a->e] = v.f; break;
    case LMN_MAP_DOUBLE:
      (*am->u.dmap)[a->e] = v.d; break;
      
    default:
      std::cerr << "Undefined map type: " << am->t <<std::endl;
      assert(0); break;
  }
  return;
}
void
lmn_arc_map_set_int_val(struct lmn_arc_map *am,
                        struct lmn_arc *a,
                        int v)
{
  assert(am!=NULL);
  assert(a!=NULL);
  assert(am->t==LMN_MAP_INT);
  (*am->u.imap)[a->e]=v;
  return;
}

  // /** set value of node N in node map NM to V */
  // void
  // lmn_node_map_set_val(struct lmn_node_map *nm,
  //                      struct lmn_node *n,
  //                      union lmn_map_val v);

  // /** set value of edge E in edge map EM to V */
  // void
  // lmn_edge_map_set_val(struct lmn_edge_map *em,
  //                      struct lmn_edge *e,
  //                      union lmn_map_val v);
  
  // /** get value of node N in node map NM */
  // union lmn_map_val
  // lmn_node_map_get_val(struct lmn_node_map *nm,
  //                      struct lmn_node *n);

  // /** get value of edge E in edge map EM */
  // union lmn_map_val
  // lmn_node_map_get_val(struct lmn_node_map *em,
  //                      struct lmn_node *e);

  // /** inquire type of node map values in NM */
  // enum lmn_map_type
  // lmn_node_map_get_type(struct lmn_node_map *nm);

  // /** inquire type of edge map values in EM */
  // enum lmn_map_type
  // lmn_node_map_get_type(struct lmn_node_map *em);
  

  // /** Get current node of a node_iterator. Returns NULL on end of iterator. */
  // struct lmn_node *
  // lmn_node_it_get(lmn_node_it *ni);
  // /** step iterator. Returns node reached after step, or NULL on end. */
  // struct lmn_node *
  // lmn_node_it_next(lmn_node_it *ni);
  // /** Get current edge of an edge_iterator. Returns NULL on end of iterator. */
  // struct lmn_edge *
  // lmn_edge_it_get(lmn_edge_it *ni);
  // /** step iterator. Returns edge reached after step, or NULL on end. */
  // struct lmn_edge *
  // lmn_edge_it_next(lmn_edge_it *ni);

/** inqure ID of node in graph */
int
lmn_digraph_node_id(struct lmn_digraph* g, lmn_node* n)
{
  assert(g!=NULL&&g->g!=nullptr);
  assert(n!=NULL);
  return g->g->id(n->n);
}

/** inquire source/target of edge */
struct lmn_node *
lmn_arc_get_source(struct lmn_digraph *g, struct lmn_arc *e)
{
  assert(g!=NULL&&g->g!=nullptr);
  assert(e!=NULL);

  const lemon::SmartDigraph::Node n = g->g->source(e->e);
  return new_lmn_node(n);  
}
/** inquire source/target of edge */
struct lmn_node *
lmn_arc_get_target(struct lmn_digraph *g, struct lmn_arc *e)
{
  assert(g!=NULL&&g->g!=nullptr);
  assert(e!=NULL);

  const lemon::SmartDigraph::Node n =g->g->target(e->e);
  return new_lmn_node(n);  
}

struct lmn_digraph*
lmn_digraph_create() {
  struct lmn_digraph *tmp = new_lmn_digraph(new lemon::SmartDigraph());
  return tmp;
}

// struct lmn_digraph*
// lmn_digraph_smart(const struct lmn_digraph *g)
// {
//   assert(g!=NULL&&g->g!=nullptr);
//   const lemon::SmartDigraph *orig = g->g;
//   lemon::SmartDigraph *res = new(lemon::SmartDigraph());
//   res->build(orig);
//   return new_lmn_digraph(res);
// }

// struct lmn_digraph*
// lmn_digraph_static(const struct lmn_digraph *c);
// {
//   assert(g!=NULL&&g->g!=nullptr);
//   lemon::concepts::Graph orig = g->g;
//   lemon::StaticDigraph *res = new(lemon::StaticDigraph());
//   res->build(orig);
//   return new_lmn_digraph(res);
// }

/** add node */
struct lmn_node *
lmn_digraph_new_node(struct lmn_digraph *g)
{
  assert(g!=NULL&g->g!=nullptr);
  const lemon::SmartDigraph::Node n = g->g->addNode();
  return new_lmn_node(n);
}

/** add arc */
struct lmn_arc *
lmn_digraph_new_arc(struct lmn_digraph *g,
                    struct lmn_node *source,
                    struct lmn_node *target)
{
  assert(g!=NULL&g->g!=nullptr);
  assert(source!=NULL);
  assert(target!=NULL);
  
  const lemon::SmartDigraph::Arc e
      = g->g->addArc(source->n,target->n);
  
  return new_lmn_arc(e);
}

struct lmn_flow_computation*
lmn_create_flow_computation(struct lmn_digraph *g,
                            struct lmn_arc_map *capacities,
                            struct lmn_node *src,
                            struct lmn_node *dst)
{
  assert(g!=NULL);
  assert(capacities!=NULL);
  assert(src!=NULL); assert(dst!=NULL);
  
  struct lmn_flow_computation *res =
      (struct lmn_flow_computation*)malloc(sizeof(struct lmn_flow_computation));
  if(res) {
    res->g = g->g;
    assert(capacities->t==LMN_MAP_INT);
    res->orig_capacities = capacities->u.imap;
    res->tmp_capacities = new lemon::SmartDigraph::ArcMap<int>(*g->g);
    // init
    for (lemon::SmartDigraph::ArcIt a(*g->g); a != lemon::INVALID; ++a) 
      res->tmp_capacities->set(a,(*capacities->u.imap)[a]);
    
    res->preflow
          = new lemon::Preflow<lemon::SmartDigraph>(*g->g,
                                                    *res->tmp_capacities,
                                                    src->n,dst->n);
  }
  return res;
}

void
free_lmn_flow_computation(struct lmn_flow_computation *c)
{
  assert(c!=NULL);
  // we don't own the graph or the orig_capacities
  delete(c->tmp_capacities);
  delete(c->preflow);
  free(c);
}
      

void
lmn_run_flow_computation(struct lmn_flow_computation *c,
                         int num_disabled,
                         lmn_arc **disabled_arcs)
{
  assert(c!=NULL);
  // kill some capacities:
  for(int i=0; i<num_disabled; i++) {
    c->tmp_capacities->set(disabled_arcs[i]->e,0);
  }
  // run init and phase 1 -- we only need the flow value, not the flow itself
  c->preflow->init();
  c->preflow->startFirstPhase();
  c->val = c->preflow->flowValue();
  // reset capacity invariant
  for(int i=0; i<num_disabled; i++) {
    c->tmp_capacities->set(disabled_arcs[i]->e,
                           (*c->orig_capacities)[disabled_arcs[i]->e]);
  }
  return;
}

double
lmn_flow_computation_get_last_val(struct lmn_flow_computation *c)
{
  return c->val;
}

int
lmn_flow_computation_flow_cmp(struct lmn_flow_computation *c, double val)
{
  if(val< c->val)
    return -1;
  else {
    if(val==c->val)
      return 0;
    else
      return +1;
  }
}
          
        
