#pragma once

#include <memory>

#include "spdlog/spdlog.h"

/** \brief This class contains one instance of an spdlogger.
 * It should be used by all classes which want to log something, so all the information is in one
 * place
 */
class logger {
   public:
    /** \brief This method returns the instance of the logger.
     * This method creates the logger when it is first called
     */
    static std::shared_ptr<spdlog::logger> get();

   private:
    /** \brief The logger instance.
     */
    static inline std::shared_ptr<spdlog::logger> _logger_instance = nullptr;
};
