#include "xls_export.h"
#include "../config/config.h"
#include "../loader/jira_loader.h"
#include <map>

namespace file_export
{
    void ExportXLS::export_status(std::vector<report::Report> &report, OpenXLSX::XLDocument &doc)
    {
       
        doc.workbook().addWorksheet("report");
        OpenXLSX::XLWorksheet wks = doc.workbook().sheet("report");

        wks.cell(1, 1).value() = "Initiative";
        wks.cell(1, 2).value() = "Cluster";
        wks.cell(1, 3).value() = "Product";
        wks.cell(1, 4).value() = "Epic";
        wks.cell(1, 5).value() = "Epic name";
        wks.cell(1, 6).value() = "Epic link";
        wks.cell(1, 7).value() = "Assigne";
        wks.cell(1, 8).value() = "fixed_version";
        wks.cell(1, 9).value() = "Status";
        wks.cell(1, 10).value() = "Day - 1";
        wks.cell(1, 11).value() = "Day - 2";
        wks.cell(1, 12).value() = "Day - 3";
        wks.cell(1, 13).value() = "Day - 4";
        wks.cell(1, 14).value() = "Day - 5";
        wks.cell(1, 15).value() = "Week - 1";
        wks.cell(1, 16).value() = "Week - 2";
        wks.cell(1, 17).value() = "Week - 3";
        wks.cell(1, 18).value() = "Month - 1";
        wks.cell(1, 19).value() = "Month - 2";
        wks.cell(1, 20).value() = "Month - 3";

        int i = 2;
        for (report::Report &r : report)
        {
            wks.cell(i, 1).value() = r.initative;
            wks.cell(i, 2).value() = r.cluster;
            wks.cell(i, 3).value() = r.product;
            int j = 4;

            for (auto [i_status, p_status] : r.issue_status)
            {
                if (j < 7)
                {
                    wks.cell(i, j++).value() = i_status.key;
                    wks.cell(i, j++).value() = i_status.name;
                    wks.cell(i, j++).formula().set("=HYPERLINK(\"https://jira.mts.ru/browse/" + p_status.key + "\")");
                    wks.cell(i, j++).value() = p_status.assigne;
                    wks.cell(i, j++).value() = p_status.fixed_version;
                }
                wks.cell(i, j++).value() = report::Report::map_status(p_status.status);
            }
            ++i;
        }
    }

    void ExportXLS::export_links(std::vector<report::Report> &report, OpenXLSX::XLDocument &doc)
    {
        doc.workbook().addWorksheet("links");
        OpenXLSX::XLWorksheet wks = doc.workbook().sheet("links");

        wks.cell(1, 1).value() = "Initiative";
        wks.cell(1, 2).value() = "Cluster";
        wks.cell(1, 3).value() = "Product";
        wks.cell(1, 4).value() = "Epic";
        wks.cell(1, 5).value() = "Epic name";
        wks.cell(1, 6).value() = "Epic link";
        wks.cell(1, 7).value() = "Assigne";
        wks.cell(1, 8).value() = "fixed_version";
        wks.cell(1, 9).value() = "Link";
        wks.cell(1, 10).value() = "Link type";
        wks.cell(1, 11).value() = "Link fixed_version";

        int i = 2;
        for (report::Report &r : report)
        {
            if (!r.issue_status.empty())
            {
                auto [i_status, p_status] = *r.issue_status.begin();
                std::vector<std::pair<std::string,std::string>> links;
                for (const auto &[lk,link_type] : p_status.links)
                    links.push_back({lk,link_type});
                //if(links.empty()) links.push_back({"",""});

                for(const auto &[lk,link_type] :links)
                {
                    
                    wks.cell(i, 1).value() = r.initative;
                    wks.cell(i, 2).value() = r.cluster;
                    wks.cell(i, 3).value() = r.product;
                    wks.cell(i, 4).value() = i_status.key;
                    wks.cell(i, 5).value() = i_status.name;
                    wks.cell(i, 6).formula().set("=HYPERLINK(\"https://jira.mts.ru/browse/" + p_status.key + "\")");
                    wks.cell(i, 7).value() = p_status.assigne;
                    wks.cell(i, 9).value() = p_status.fixed_version;
                    wks.cell(i, 9).value() = "https://jira.mts.ru/browse/" + lk;
                    wks.cell(i, 10).value() = link_type;
                    try{
                        auto link_jira = loaders::LoaderJira::get().load(lk,Config::get().get_confluence_identity());
                    if(link_jira) wks.cell(i, 10).value() = link_jira->get_fixed_version();
                    }catch(...){}
                    ++i;
                }
            }
            
        }
    }

    void ExportXLS::start_export(std::vector<report::Report> &report)
    {

        // create description XLS
        std::string xls_name = "report.xlsx";

        OpenXLSX::XLDocument doc;
        doc.create(xls_name);

        export_status(report, doc);
        export_links(report, doc);

        doc.workbook().deleteSheet("Sheet1");
        doc.save();
        doc.close();
    }
}