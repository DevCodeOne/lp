#pragma once

#include <mutex>

#include "bcm_host.h"

/** \brief This class will initialize the bcm_host library.
 */
class bcm_host {
   public:
    /** \brief Intialize the library.
     * It will only be initialized when the method is called first.
     * Further calls will be ignored
     */
    static void initialize();

   private:
    /** \brief Variable which contains if the library is initialized or not.
     */
    static inline bool _is_initialized = false;
    /** \brief Mutex to exclude simulatenous access to the is_initialized variable.
     */
    static inline std::mutex _bcm_mutex;
};
