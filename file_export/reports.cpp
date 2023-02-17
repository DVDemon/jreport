#include "reports.h"
#include <map>

namespace report{

    std::string Report::map_status(const std::string & old_status){
         static std::map<std::string, std::string> status{
            {"Бэклог", "Бэклог"},
            {"Выполнено", "Готово"},
            {"Закрыт", "Готово"},
            {"Открыто", "В работе"},
            {"Готово к проверке", "Ревью"},
            {"В ожидании", "Приостановлено"},
            {"Отклонено", "Отменено"}};

            if (status.find(old_status) != std::end(status))
            {
                return status[old_status];
            }
            else
                return old_status;
     }

}