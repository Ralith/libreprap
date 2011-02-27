(ns reprap-clj
  (:use [clj-native.direct :only [defclib loadlib typeof]]
        [clj-native.structs :only [byref byval]]
        [clj-native.callbacks :only [callback]]))

(defclib
  libreprap
  (:libname "libreprap")
  (:structs
   (packed :s1 short :s2 short))
  (:unions
   (splitint :theint int :packed packed))
  (:callbacks
   (add-cb [int int] int))
  (:functions
   (rr_enumerate_ports [] constchar**)))

(loadlib libreprap)
