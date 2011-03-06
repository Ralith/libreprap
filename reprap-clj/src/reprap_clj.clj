(ns reprap-clj
  (:use [clj-native.direct :only [defclib loadlib typeof]]
        [clj-native.structs :only [byref byval]]
        [clj-native.callbacks :only [callback]]
        [net.n01se.clojure-jna.libc-utils :only [select]]))

(defclib
  libreprap
  (:libname "reprap")
  (:callbacks
   (sendcb [void* void* void* constchar* size_t] void)
   (recvcb [void* void* constchar* size_t] void)
   (replycb [void* void* void* float] void)
   (boolcb [void* void* char] void)
   (errcb [void* void* int constchar* size_t] void))
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
   (rr_reset [void*] void) ;; rr_dev
   (rr_close [void*] int) ;; rr_dev
   (rr_free [void*] void) ;; rr_dev
   (rr_enqueue [void* ;; rr_dev
		enum ;; rr_prio
		void* ;; cbdata
                constchar* ;; block
                size_t] ;; block size
	       void)
   (rr_handle_readable [void*] int) ;; rr_dev
   (rr_handle_writable [void*] int) ;; rr_dev
   (rr_flush [void*] int) ;; rr_dev
   (rr_dev_fd [void*] int) ;; rr_dev
   (rr_dev_lineno [void*] long) ;;rr_dev
   (rr_dev_buffered [void*] int))) ;; rr_dev

(loadlib libreprap)

;; Enums
(def rr-proto (zipmap [:simple :fived :tonokip] (range)))
(def rr-prio (zipmap [:normal :high :resend :count] (range)))
(def rr-error (zipmap [:block-too-large :write-failed :unsupported-proto
                       :unknown-reply :uncached-resend :hardware-fault
                       :unsent-resend :malformed-resend-request] (range -1 -100 -1)))
(def rr-reply (zipmap [:ok :nozzle-temp :bed-temp
                       :x-pos :y-pos :z-pos :e-pos] (range)))

(defn enumerate-ports []
  (.getStringArray (rr_enumerate_ports) 0))

;; (def default-callbacks

(defn create-dev [& {:keys [proto onsend onsend-d onrecv onrecv-d onreply onreply-d onerr onerr-d
                        want-writable want-writable-d resend-cache-size]
                     :or {proto :fived
                          resend-cache-size 64
                          want-writable (callback boolcb (fn [& TODO] (println "want_writable called")))
                          onrecv (callback recvcb (fn [dev data line size]
                                                    (println "Received: " (subs line 0 size))))}}]
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

(defmacro access-dev [dev & code]
  `(locking @~dev
     (let [~dev @~dev]
       (when (not ~dev)
         (throw (Exception. "Null pointer dereference")))
       ~@code)))

(defn reset-dev [dev]
  (locking @dev
    (when (not @dev)
      (throw (Exception. "Null pointer dereference")))
    (rr_reset @dev)))

(defn dev->fd [dev]
  (access-dev dev
    (rr_dev_fd dev)))

(defn dev->lineno [dev]
  (access-dev dev
    (rr_dev_lineno [dev])))

(defn int2bool [i]
  (not (= i 0)))

(defn dev-buffered? [dev]
  (access-dev dev
    (int2bool (rr_dev_buffered dev))))

(defn open-dev [dev port speed]
  (access-dev dev
    (rr_open dev port speed)))

(defn close-dev [dev]
  (access-dev dev
    (rr_close dev)))
  
(defn enqueue [dev prio block & [cbdata]]
  {:pre (< 0 count block)}
  (access-dev dev (rr_enqueue dev (rr-prio prio) nil block (count block))))

(defn handle-readable [dev]
  (access-dev dev
    (rr_handle_readable dev)))

(defn handle-writable [dev]
  (access-dev dev
    (rr_handle_writable dev)))

(defn flush-queue [dev]
  (access-dev dev
    (rr_flush dev)))

(defn handle-all [dev]
  (let [fd (dev->fd dev)
        [readfds writefds errfds] (select [fd] [fd] [] 10)]
    (when (readfds fd)
      (handle-readable dev))
    (when (writefds fd)
      (handle-writable dev))))

(defn comms-loop [dev lines]
  (loop [lines lines]
    (let [line (first lines)]
      (if (< 0 (count line))
        (do (enqueue dev :normal line)
            (println "Enqueued:" line)
            (Thread/sleep 100)
            (handle-all dev)
            (recur (rest lines)))
        (do
          (flush-queue dev)
          (handle-all dev))))))

(defn test-main []
  (let [dev (create-dev)]
    (open-dev dev "/dev/ttyACM0" 115200)
    (handle-all dev)
    ;(handle-readable dev)
    ;(enqueue dev :normal "M105\n")
    ;(handle-writable dev)
    ;(handle-readable dev)
    ;(enqueue dev :normal "M105\n")
    ;(handle-writable dev)
    ;(handle-readable dev)
    ;(handle-writable dev)
    ;(handle-readable dev)
    (comms-loop dev ["M105\n" "G1 F10 X10\n" "M105\n"])
    (close-dev dev)
    (free-dev dev)))
