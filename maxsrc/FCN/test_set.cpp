//
// Created by max on 19.09.2020.
//


float rowData[]={1,2,3,4,5,6,7,8};

static char incROW() {
    for(unsigned int i=0; i<sizeof(rowData)/sizeof(rowData[0]); i++) {
        rowData[i]+=0.01;
    }
    return '+';
}

int main() {
    rollingdata data;
    data.Allocate(3, 5, 4);

    data.Append(rowData);
    data.Print();

    std::cout << incROW() << std::endl;
    data.Append(rowData+1);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+2);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+3);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+4);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+3);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+2);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+3);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+2);
    data.Print();
    std::cout << incROW() << std::endl;
    data.Append(rowData+1);
    data.Print();
    std::cout << incROW() << std::endl;
    for(int i=0; i<50; i++) {
        data.Append(rowData);
        data.Print();
        std::cout << incROW() << std::endl;
    }
}