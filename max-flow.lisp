
(in-package :de.uuhaus.lemon)

;; max flow problem with changeable capacities
(defclass max-flow-problem ()
  (;; internal data structures are wrapped lemon objects
   (graph :reader get-graph :initarg :graph)
   (source :reader get-source :initarg :source)
   (target :reader get-target :initarg :target)
   ;;; lispy data structures:
   ;; lemon-nodes at I contains the node for lisp node id I
   (lemon-nodes :accessor get-lemon-nodes
		:type simple-vector
		:initarg :lemon-nodes)
   (arcs :accessor get-arcs :type simple-vector
	 :initarg :arcs)
   ;; lemon arcs, in the order of the lisp arcs
   (lemon-arcs :accessor get-lemon-arcs :type simple-vector
	       :initarg :lemon-arcs)
   (interdictable-arcs :reader get-interdictable-arcs
		       :initarg :interdictable-arcs)
   (interdiction-costs :reader get-interdiction-costs
		       :type simple-vector
		       :initarg :interdiction-costs)
   (interdiction-probs :reader get-interdiction-probs
		       :type simple-vector
		       :initarg :interdiction-probs)
   (arc-capacities :reader get-arc-capacities
		   :type simple-vector
		   :initarg :arc-capacities)
   (lemon-arc-capacities :reader get-lemon-arc-capacities
			 :initarg :lemon-arc-capacities)
   ;; suspended max flow computation routine:
   (lemon-flow-computation :reader get-lemon-flow-computation
			   :initarg :lemon-flow-computation)
   ;; foreign array to avoid reallocation of interdiction status
   (foreign-interdiction-array :reader get-foreign-interdiction-array
			       :initarg :foreign-interdiction-array)
   )
  (:documentation ""))

(defun make-snip-instance (&key
			     edges
			     arc-interdiction-cost 
			     arc-interdiction-success-prob 
			     interdictable-arcs 
			     arc-capacities)
  ;; we require lisp nodes 0..MAX-NODE-ID to make indexing of the lemon-nodes
  ;; array fast
  (let ((max-node-id (loop :for (n1 . n2) :in edges
			:maximizing (max n1 n2))))
    (let* ((g (lmn-digraph-create))
	   (lemon-nodes (coerce
			 (loop :repeat (+ 1 max-node-id)
			    :collecting (lmn-digraph-new-node g))
			 'simple-vector))
	   (lemon-arcs  (coerce
			 (loop :for e :in edges
			    :collecting (lmn-digraph-new-arc
					 g
					 (svref lemon-nodes (car e))
					 (svref lemon-nodes (cdr e))))
			 'simple-vector))
	   (lemon-capacities (let ((m (lmn-digraph-new-arc-map g :LMN-MAP-INT)))
			       (loop :for e :across lemon-arcs
				  :for (dummy . cap) in arc-capacities
				  :do (lmn-arc-map-set-int-val m e cap))
			       m))
	   (lemon-src (svref lemon-nodes 0))
	   (lemon-dst (svref lemon-nodes (- (length lemon-nodes) 1)))
	   (lemon-flow-computation (lmn-create-flow-computation
				    g lemon-capacities lemon-src lemon-dst))
	   (foreign-interdiction-array (cffi::foreign-array-alloc
					(make-array
					 (length interdictable-arcs)
					 :initial-element (cffi:null-pointer))
					`(:array lmn-arc ,(length interdictable-arcs))))
	   )
      (let ((res (make-instance
		  'max-flow-problem
		  :graph g
		  :lemon-nodes lemon-nodes
		  :arcs (coerce edges 'simple-vector)
		  :lemon-arcs lemon-arcs
		  :interdictable-arcs (coerce interdictable-arcs
					      'simple-vector)
		  :interdiction-costs (map 'simple-vector
					   #'cdr
					   arc-interdiction-cost)
		  :interdiction-probs (map 'simple-vector
					   #'cdr
					   arc-interdiction-success-prob)
		  :arc-capacities (map 'simple-vector
				       #'cdr
				       arc-capacities)
		  :lemon-arc-capacities lemon-capacities
		  :source lemon-src
		  :target lemon-dst
		  :lemon-flow-computation lemon-flow-computation
		  :foreign-interdiction-array foreign-interdiction-array)))
	(tg:finalize
	 res
	 #'(lambda ()
	     (cffi::foreign-array-free foreign-interdiction-array)
	     (free-lmn-flow-computation lemon-flow-computation)
	     (loop :for e :across lemon-arcs
		:when (and e (not (cffi:null-pointer-p e)))
		:do (free-lmn-arc e))
	     (loop :for n :across lemon-nodes
		:when (and n (not (cffi:null-pointer-p n)))
		:do (free-lmn-node n))
	     (when (and g (not (cffi:null-pointer-p g)))
	       (free-lmn-digraph g))
	     (format T ";; possibly incomplete dtor for max-flow-problem~%")))
	
	res))))

(defun make-hardcoded-snip-instance (edge/cap/cost/prob-list)
  "Take a list of (node1 node2 capacity cost prob) entries (for interdictable arcs)
and (node1 node2 capacity) entries for non-interdictable ones and generate a
hardcoded-snip-instance from them."
  (let ((edges '())
        (capacities '())
        (costs '())
        (probs '()))
    (dolist (e/c/c/p edge/cap/cost/prob-list)
      (destructuring-bind (n1 n2 cap &optional cost prob)
          e/c/c/p
        (let ((e (cons n1 n2)))
          (push e edges)
          (setf capacities (acons e cap capacities))
          (when cost
            (setf costs (acons e cost costs)))
          (when prob
            (setf probs (acons e prob probs))))))
    (make-snip-instance
     :edges edges
     :arc-interdiction-cost costs
     :arc-interdiction-success-prob probs
     :interdictable-arcs (mapcar #'car probs)
     :arc-capacities capacities
     )))

(defparameter +cmw-snip-4x9+
  (let ((infty 100000)
        (prob 0.75d0)
        (cost 1))
    (make-hardcoded-snip-instance 
     `((0 1 ,infty) (0 2 ,infty) (0 3 ,infty) (0 4 ,infty) 
       (1 2 ,infty)           (2 3 ,infty)           (3 4 ,infty)
       (1 5 20)               (2 6 60)               (3 7 20)               (4 8 10 ,cost ,prob)
       (5 6 20)               (7 6 20 ,cost ,prob)   (7 8 10)
       (5 9 20 ,cost ,prob)   (6 10 70)              (7 11 20)              (8 12 10)
       (9 10 20 ,cost ,prob)  (10 11 20)             (11 12 10 ,cost ,prob)
       (9 13 20 ,cost ,prob)  (10 14 80 ,cost ,prob) (11 15 10)             (12 16 10 ,cost ,prob)
       (14 13 20 ,cost ,prob) (14 15 20 ,cost ,prob) (15 16 10)
       (13 17 20 ,cost ,prob) (14 18 80)             (15 19 30)             (16 20 10)
       (18 17 20)             (18 19 50)             (19 20 10)
       (17 21 20 ,cost ,prob) (18 22 30 ,cost ,prob) (19 23 40)             (20 24 10)
       (21 22 20 ,cost ,prob) (22 23 20)             (23 24 10)
       (21 25 20 ,cost ,prob) (22 26 30 ,cost ,prob) (23 27 50)             (24 28 10 ,cost ,prob)
       (25 26 20)             (26 27 50)             (27 28 10)
       (25 29 20 ,cost ,prob) (26 30 50)             (27 31 50)             (28 32 10 ,cost ,prob)
       (29 30 20)             (30 31 50 ,cost ,prob) (31 32 10 ,cost ,prob)
       (29 33 20 ,cost ,prob) (30 34 70)             (31 35 40 ,cost ,prob) (32 36 10 ,cost ,prob)
       (33 34 ,infty)        (34 35 ,infty)       (35 36 ,infty)
       (33 37 ,infty) (34 37 ,infty) (35 37 ,infty) (36 37 ,infty)))))


(defun run-interdicted-max-flow (instance interdictions)
  "Run MAX-FLOW on instance, return flow value.
INTERDICTIONS specifies the arcs to be interdicted.
"
  (declare (optimize (speed 3)))
  (let ((interdiction-array (get-foreign-interdiction-array instance))
	(num-interdicted 0))
    (declare (type (integer 0) num-interdicted))
    ;; FIXME: adapt for type of interdictions
    (loop :with lemon-arcs :of-type simple-vector := (get-lemon-arcs instance)
       :with lisp-arcs :of-type simple-vector := (get-arcs instance)
       :for thing :in interdictions
       :do (let ((lemon-arc nil))
	     (etypecase thing
	       (cons (let ((idx (position thing lisp-arcs :test #'equal)))
		       (setf lemon-arc (svref lemon-arcs idx)))))
	     (setf (cffi:mem-aref interdiction-array 'lmn-arc num-interdicted)
		   lemon-arc)
	     (incf num-interdicted)))
    (lmn-run-flow-computation (get-lemon-flow-computation instance)
			      num-interdicted
			      interdiction-array)
    (values)))


(defun random-outage-test (instance)
  "Run max-flow tests with random outages"
  (declare (optimize (speed 3)))
  (let* ((candidates (get-interdictable-arcs instance))
	 (num-arcs (length candidates))
	 (num-interdicted (random num-arcs))
	 (interdicted (loop :repeat num-interdicted
			 :as idx := (random num-arcs)
			 :collecting (svref candidates idx))))
    (declare (dynamic-extent interdicted)
	     (type simple-vector candidates))
    (run-interdicted-max-flow instance interdicted)))
