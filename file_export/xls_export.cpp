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
        wks.cell(1, 7).value() = "Status";
        wks.setColor(OpenXLSX::XLColor(120,0,0));
        int i = 2;
        for (report::Report &r : report)
        {
            wks.cell(i, 1).value() = r.initative;
            wks.cell(i, 2).value() = r.cluster;
            wks.cell(i, 3).value() = r.product;
            int j = 4;

            for(auto [i_status,p_status]: r.issue_status){
                wks.cell(i, j++).value() = i_status.key;
                wks.cell(i, j++).value() = i_status.name;
                wks.cell(i, j++).formula().set("=HYPERLINK(\"https://jira.mts.ru/browse/"+p_status.key+"\")");
                wks.cell(i, j++).value() = map_status(p_status.status);
                
                
            }
            ++i;
        }

        doc.workbook().deleteSheet("Sheet1");
        doc.save();
        doc.close();
    }
}