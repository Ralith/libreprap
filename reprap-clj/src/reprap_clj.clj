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
   (boolcb [void* void* char] void)
   (errcb [void* void* int constchar* size_t]))
  (:functions
   (rr_enumerate_ports [] void*)
   (rr_create [enum ;; rr_proto
	       sendcb void* ;; onsend & data
	       recvcb void* ;; onrecv & data
	       replycb void* ;; onreply & data
	       errcb void* ;; onerr & data
	       boolcb void* ;; want_writable & data
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

(defn create-dev [& {:keys [proto onsend onsend-d onrecv onrecv-d onreply onreply-d onerr onerr-d
                        want-writable want-writable-d resend-cache-size]
                 :or {proto :fived resend-cache-size 64}}]
  (atom (rr_create (rr-proto proto) onsend onsend-d onrecv onrecv-d onreply onreply-d
             onerr onerr-d want-writable want-writable-d resend-cache-size)))

(defn free-dev [dev]
  (locking @dev
    (swap! dev #(do
                  (when (not %)
                    (throw (Exception. "Double free")))
                  (rr_free %)
                  nil))))

(defn test-free-locking [dev]
  (future
    (locking @dev
      (swap! dev #(do
                    (println "sleeping")
                    (Thread/sleep 5000)
                    (when (not %)
                      (throw (Exception. "Double free")))
                    (rr_free %)
                    (println "freed")
                    nil))))
  (free-dev dev))

(defn reset-dev [dev]
  (locking @dev
    (when (not @dev)
      (throw (Exception. "Null pointer dereference")))
    (rr_reset @dev)))
