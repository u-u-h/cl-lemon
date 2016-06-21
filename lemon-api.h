/* -*- mode:c -*- */
/** @file
 ** @brief thin C-language interface to lemon graph library.
 **
 ** This header exposes an API to the eiger code intended to be useful
 ** to wrap into a scripting language interface or inclusion into other projects
 **
 ** we currently only provide just enough to build a graph and run max
 ** flow with weight modifications
 **
 ** (c) 2016 Utz-Uwe Haus
 */

#ifndef LEMON_API_H_
#define LEMON_API_H_ 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /* opaque wrappers that hide the implementation detail of the graph or node */
  struct lmn_digraph;
  struct lmn_node;
  struct lmn_node_it;
  struct lmn_node_map;
  struct lmn_arc;
  struct lmn_node_it;
  struct lmn_egde_map;

  /** node and arc maps can have values of these types: */
  enum lmn_map_type {
    LMN_MAP_INT,
    LMN_MAP_UINT,
    LMN_MAP_LONG,
    LMN_MAP_ULONG,
    LMN_MAP_FLOAT,
    LMN_MAP_DOUBLE
  };

  /** node and arc maps accept this unifying value type: */
  union lmn_map_val {
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    float f;
    double d;
  };

  /* ctors */
  /** default: build a SmartDigraph */
  struct lmn_digraph*
  lmn_digraph_create();

  /** add node */
  struct lmn_node *
  lmn_digraph_new_node(struct lmn_digraph *g);

  /** add arc */
  struct lmn_arc *
  lmn_digraph_new_arc(struct lmn_digraph *g,
                      struct lmn_node *source,
                      struct lmn_node *target);

  struct lmn_arc_map*
  lmn_digraph_new_arc_map(struct lmn_digraph *g,
                          enum lmn_map_type t);
  
  /* dtors */
  void free_lmn_digraph(struct lmn_digraph* g);
  void free_lmn_node(struct lmn_node* n);
  void free_lmn_node_it(struct lmn_node_it* ni);
  void free_lmn_node_map(struct lmn_node_map* nm);
  void free_lmn_arc(struct lmn_arc* e);
  void free_lmn_arc_it(struct lmn_arc_it* ei);
  void free_lmn_arc_map(struct lmn_arc_map* em);
  void free_lmn_flow_computation(struct lmn_flow_computation*);

  /** set value of node N in node map NM to V */
  void
  lmn_node_map_set_val(struct lmn_node_map *nm,
                       struct lmn_node *n,
                       union lmn_map_val v);

  /** set value of arc E in arc map EM to V */
  void
  lmn_arc_map_set_val(struct lmn_arc_map *em,
                       struct lmn_arc *e,
                       union lmn_map_val v);
  void
  lmn_arc_map_set_int_val(struct lmn_arc_map *em,
                          struct lmn_arc *e,
                          int v);
  
  /** get value of node N in node map NM */
  union lmn_map_val
  lmn_node_map_get_val(struct lmn_node_map *nm,
                       struct lmn_node *n);

  /** get value of arc E in arc map EM */
  union lmn_map_val
  lmn_node_map_get_val(struct lmn_node_map *em,
                       struct lmn_node *e);

  /** inquire type of node map values in NM */
  enum lmn_map_type
  lmn_node_map_get_type(struct lmn_node_map *nm);

  /** inquire type of arc map values in EM */
  enum lmn_map_type
  lmn_node_map_get_type(struct lmn_node_map *em);
  

  /** Get current node of a node_iterator. Returns NULL on end of iterator. */
  struct lmn_node *
  lmn_node_it_get(lmn_node_it *ni);
  /** step iterator. Returns node reached after step, or NULL on end. */
  struct lmn_node *
  lmn_node_it_next(lmn_node_it *ni);
  /** Get current arc of an arc_iterator. Returns NULL on end of iterator. */
  struct lmn_arc *
  lmn_arc_it_get(lmn_arc_it *ni);
  /** step iterator. Returns arc reached after step, or NULL on end. */
  struct lmn_arc *
  lmn_arc_it_next(lmn_arc_it *ni);

  /** inqure ID of node in graph */
  int
  lmn_digraph_node_id(struct lmn_digraph* g, lmn_node* n);

  /** inquire source/target of arc */
  struct lmn_node *
  lmn_arc_get_source(struct lmn_arc *e);
  struct lmn_node *
  lmn_arc_get_target(struct lmn_arc *e);

  /**** algorithms ****/
  struct lmn_flow_computation;
  struct lmn_flow_computation *
  lmn_create_flow_computation(struct lmn_digraph *g,
                              struct lmn_arc_map *capacities,
                              struct lmn_node *src,
                              struct lmn_node *dst);
  /** compute max flow, setting capacities for DISABLED_ARCS temporarily
      to 0
      DISABLED_ARCS must habe #arcs entries or be NULL.
      Result is not returned (to avoid overhead of allocating a double on the caller side
      if not needed) but stored in C and can be retrieved by lmn_flow_computation_get_last_val.
  */
  void
  lmn_run_flow_computation(struct lmn_flow_computation *c,
                           int num_disabled,
                           lmn_arc **disabled_arcs);

  double
  lmn_flow_computation_get_last_val(struct lmn_flow_computation *c);

  /* check last value against VAL. Return negative for <, 0 for =, positive for > */
  int
  lmn_flow_computation_flow_cmp(struct lmn_flow_computation *c, double val);
  

#ifdef __cplusplus
} // extern "C"
#endif
