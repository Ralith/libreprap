(ns reprap-clj
  (:use [clj-native.direct :only [defclib loadlib typeof]]
        [clj-native.structs :only [byref byval]]
        [clj-native.callbacks :only [callback]]))

(defclib
  libreprap
  (:libname "reprap")
  (:structs
   (packed :s1 short :s2 short))
  (:unions
   (splitint :theint int :packed packed))
  (:callbacks
   (sendcb [void* void* void* constchar* size_t] void)
   (recvcb [void* void* constchar* size_t] void)
   (replycb [void* void* void* float] void)
   (boolcb [void* void* char] void))
  (:functions
   (rr_enumerate_ports [] void*)
   (rr_create [enum ;; rr_proto
	       void* void* ;; onsend & data
	       void* void* ;; onrecv & data
	       void* void* ;; onreply & data
	       void* void* ;; onerr & data
	       void* void* ;; want_writable & data
	       size_t] ;; resend_cache_size
	      void*) ;; rr_dev
   (rr_open [void* ;; rr_dev
	     constchar* ;; port
	     long] ;; speed
	    int)
   (rr_reset [void*] int) ;; rr_dev
   (rr_close [void*] int) ;; rr_dev
   (rr_free [void*] int) ;; rr_dev
   (rr_enqueue [void* ;; rr_dev
		enum ;; rr_prio
		void* ;; cbdata
                constchar* ;; block
                size_t] ;; block size
	       void)))

(loadlib libreprap)

(def rr-proto (zipmap [:simple :fived :tonokip] (range)))

(defn enumerate-ports []
  (.getStringArray (rr_enumerate_ports) 0))

(defn create [proto & TODO]
  (rr_create (rr-proto proto) 0 0 0 0 0 0 0 0 0 0 64))

