/* SWIG wrapper around a minimal lemon interface */
%module "lemon-api"
%{
#include "lemon-api.h"
%}
/* the bare interface should be fine to start with any language SWIG supports,
 *   but for some we provide special sugar
 */
#if defined(SWIGCFFI)
%feature("intern_function","de.uuhaus.swig:swig-lispify"); /* using swig-lispify */
%insert(lisphead) %{
  (cl:defpackage :de.uuhaus.swig (:export #:swig-lispify))
  (cl:in-package :de.uuhaus.swig)
%}

%insert(swiglisp) %{
  (cl:defpackage :de.uuhaus.lemon-api (:nicknames :lemon-api)
   (:use :cl :trivial-garbage)
   (:export
    #:lmn-digraph
    #:lmn-node
    #:lmn-arc
    #:lmn-digraph-create
    #:lmn-digraph-new-node
    #:lmn-digraph-new-arc
    #:lmn-digraph-new-arc-map
    #:lmn-arc-map-set-val
    #:lmn-arc-map-set-int-val
    #:lmn-create-flow-computation
    #:lmn-flow-computation
    #:lmn-run-flow-computation
    #:free-lmn-flow-computation
    #:free-lmn-digraph
    #:free-lmn-node
    #:free-lmn-arc
  ))
  (cl:eval-when (:compile-toplevel :load-toplevel :execute)
   (cl:declaim (cl:optimize (cl:speed 3))))

  ;; someone needs to do the equivalent of
  ;;  (ql:quickload "cffi")
  ;;  (cffi:load-foreign-library "libeiger")
  ;; e.g. in an .asd file
  (cl:in-package :de.uuhaus.lemon-api)
  (cffi:defctype lmn-digraph :pointer)
  (cffi:defctype lmn-node :pointer) 
  (cffi:defctype lmn-arc :pointer)
  (cffi:defctype lmn-arc-map :pointer)
  (cffi:defctype lmn-flow-computation :pointer)
  ;; duplication of the union lmn_map_val
  (cffi:defcunion lmn-map-val
   (int-val :int)
   (uint-val :unsigned-int)
   (long-val :long)
   (ulong-val :unsigned-long)
   (float-val :float)
   (double-val :double))


%}

%typemap(cin)  struct lmn_digraph* "lmn-digraph";
%typemap(cout) struct lmn_digraph* "lmn-digraph";

%typemap(cin)  struct lmn_node* "lmn-node";
%typemap(cout) struct lmn_node* "lmn-node";

%typemap(cin)  struct lmn_arc* "lmn-arc";
%typemap(cout) struct lmn_arc* "lmn-arc";

%typemap(cin)  struct lmn_digraph* "lmn-arc-map";
%typemap(cout) struct lmn_digraph* "lmn-arc-map";

%typemap(cin)  struct lmn_flow_computation* "lmn-flow-computation";
%typemap(cout) struct lmn_flow_computation* "lmn-flow-computation";




#endif

%include "lemon-api.h"
