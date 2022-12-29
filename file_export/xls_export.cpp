#include "xls_export.h"
#include <experimental/filesystem>
#include <OpenXLSX/OpenXLSX.hpp>
#include <map>

namespace file_export
{
    void ExportXLS::start_export(std::vector<report::Report> &report)
    {
        std::map<std::string,std::string> status;

        status["Бэклог"]="В работе";
        status["Выполнено"]="Готово";
        status["Закрыт"]="Готово";
        status["Открыто"]="В работе";
        status["Готово к проверке"]="Ревью";
        status["В ожидании"]="Приостановлено";
        status["Отклонено"]="Отменено";

        auto map_status = [&status](auto &old_status) -> std::string {
            if(status.find(old_status)!=std::end(status)){
                return status[old_status];
            } else return old_status;
        };

        // create description XLS
        std::string xls_name = "report.xlsx";

        OpenXLSX::XLDocument doc;
        doc.create(xls_name);


        doc.workbook().addWorksheet("report");
        OpenXLSX::XLWorksheet wks = doc.workbook().sheet("report");


        wks.cell(1, 1).value() = "Initiative";
        wks.cell(1, 2).value() = "Cluster";
        wks.cell(1, 3).value() = "Product";
        wks.cell(1, 4).value() = "Epic";
        wks.cell(1, 5).value() = "Epic name";
        wks.cell(1, 6).value() = "Epic link";
        wks.cell(1, 7).value() = "Assigne";
        wks.cell(1, 8).value() = "Status";
        wks.cell(1, 9).value() = "Day - 1";
        wks.cell(1, 10).value() = "Day - 2";
        wks.cell(1, 11).value() = "Day - 3";
        wks.cell(1, 12).value() = "Day - 4";
        wks.cell(1, 13).value() = "Day - 5";
        wks.cell(1, 14).value() = "Week - 1";
        wks.cell(1, 15).value() = "Week - 2";
        wks.cell(1, 16).value() = "Week - 3";
        wks.cell(1, 17).value() = "Month - 1";
        wks.cell(1, 18).value() = "Month - 2";
        wks.cell(1, 19).value() = "Month - 3";
        wks.setColor(OpenXLSX::XLColor(120,0,0));
        int i = 2;
        for (report::Report &r : report)
        {
            wks.cell(i, 1).value() = r.initative;
            wks.cell(i, 2).value() = r.cluster;
            wks.cell(i, 3).value() = r.product;
            int j = 4;

            for(auto [i_status,p_status]: r.issue_status){
                if(j<6){
                    wks.cell(i, j++).value() = i_status.key;
                    wks.cell(i, j++).value() = i_status.name;
                    wks.cell(i, j++).formula().set("=HYPERLINK(\"https://jira.mts.ru/browse/"+p_status.key+"\")");
                    wks.cell(i, j++).value() = p_status.assigne;
                }
                wks.cell(i, j++).value() = map_status(p_status.status);                
            }
            ++i;
        }

        doc.workbook().deleteSheet("Sheet1");
        doc.save();
        doc.close();
    }
}