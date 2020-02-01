#include "gnuplot.h"

//
// Created by Stefano Vannucchi on 15/01/2020.
//

int main() {
    GnuplotPipe gp;
    gp.sendLine("plot(\"elements-result.txt\") with points palette");
    return 0;
}

