#include <iostream>
#include "model/product.h"
#include <OpenXLSX/OpenXLSX.hpp>

auto main() -> int
{

    std::string xls_name = "products.xlsx";

    OpenXLSX::XLDocument doc;
    doc.create(xls_name);
    doc.workbook().addWorksheet("report");
    OpenXLSX::XLWorksheet wks = doc.workbook().sheet("report");

    wks.cell(1, 1).value() = "Cluster";
    wks.cell(1, 2).value() = "Product";
    size_t i = 2;
    for (auto &[name, product] : model::Products::get().products())
    {
        wks.cell(i, 1).value() = product->cluster;
        wks.cell(i, 2).value() = product->name;
        ++i;
    }
    doc.workbook().deleteSheet("Sheet1");
    doc.save();
    doc.close();
    return 1;
}