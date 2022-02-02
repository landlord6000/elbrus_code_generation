#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

#define qpfadds  "__builtin_e2k_qpfadds"
#define qpfmuls  "__builtin_e2k_qpfmuls"
#define qpfhadds "__builtin_e2k_qpfhadds"

#define a "a"
#define b "b"
#define c "c"
#define bs "bs"

int gen_code(               \
    int n, int m, \
    std::vector<std::string> func,          \
    std::vector<std::string> args,          \
    std::vector<int> dim,              \
    std::vector<int> init,             \
    std::vector<int> incr_n,           \
    std::vector<int> incr_m            \
    ){

    // arguments:
    // n - размер блока
    // m - число блоков
    // func - массив с дескрипторами функций (ну типа дескрипторы)
    //
    //-------- default register names
    // для input map имя регистра - a, для output map - c
    // для weights - b, для biases - bs
    // имена регистров образуются следующим образом:
    // если размерность = 1, то регистры получат следующие имена:
    // a1, a2, a3,...
    // если размерность = 2, то 
    // a1_1, a1_2, ..., a1_n, a2_1, ... am_n; и так далее

    std::ofstream fout; fout.open("code.txt");

    int func_am = func.size();
    int func_count = 0;

    int args_am = args.size();
    int args_count = 0;

    auto dim_sum = std::accumulate(dim.begin(), dim.end(), 0);
    if(dim_sum != init.size() || dim_sum != incr_n.size() || dim_sum != incr_m.size() || dim.size() != args.size()) std::cerr << "input dara error\n";
    if(args_am != func_am + 2) std::cerr << "amount of functions' \")\" (" << func_am << ") does not match the number of arguments (" << args_am << "), output might be wrong\n";

    for(int xa = 0; xa < m; xa++){
        for(int fd = 0; fd < n; fd++){
            int init_incr = 0;
            
            fout << args[0];
            for(int j = 0; j < dim[0]; ++j){ 
                fout << init[j + init_incr];
                if(j+1 != dim[0])
                    fout << "_";
                else
                     fout << " = ";
                init_incr++;
            }
            args_count++;

            for(int ff = 0; ff < func_am; ++ff){
                fout << func[ff] << "("; // f1_name(f2_name(...
            }

            if(args_am > 1){
                for(int k = 1; k < 3; ++k){
                    fout << args[k];
                    for(int j = 0; j < dim[k]; ++j){ 
                        fout << init[init_incr];
                        if(j+1 != dim[k])
                            fout << "_";
                        else
                            if(k == 1)
                                fout << ", "; 
                            else    
                                fout << "), ";                                          
                        init_incr++;
                    }
                    args_count++;
                }
                func_count++;
            }
            else
                std::cerr << "amount of args is less than 2\n";

            for(int aa = 3; aa < args_am; ++aa){
                for(int j = 0; j < dim[aa]; ++j){ 
                    fout << args[aa];
                    fout << init[j + init_incr];
                    if(j+1 != dim[aa])
                        fout << "_";
                    else
                        if(aa != args_am - 1) 
                            fout << "), ";                                            // , 
                        else
                            fout << ");";
                    init_incr++;
                }
                args_count++; 
                func_count++;
                fout << "\n";
            }
            for(int i = 0; i < init.size(); ++i){ 
                printf("%d ", init[i]);
                init[i] =  init[i] + incr_n[i];  
            }
            printf("\n");
          
            if(func_count != func_am) std::cerr << "amount of functions' \")\" (" << func_count << ") is not correct (should be " << func_am << "), output might be wrong\n";
            if(args_count != args_am) std::cerr << "amount of arguments (" << args_count << ") is not correct (should be " << args_am << "), output might be wrong\n";
            func_count = 0; args_count = 0;
        }
        for(int i = 0; i < init.size(); ++i){ 
            init[i] = 0;  
            init[i] = incr_m[i]*(xa+1);
        }


        fout << "\n";
    }

    fout.close();

}

int main(int argc, char* argv[]){

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    std::vector<std::string> func = {qpfadds, qpfmuls};
    std::vector<std::string> args = {c, a, b, bs};
    std::vector<int> dim     = {2, 2, 2, 1}; 
    std::vector<int> init    = {0, 0,  0, 0,  0, 0,  0};  // начинаем не с первого аргумента, а с левой части равенства
    std::vector<int> incr_n  = {0, 1,  1, 0,  0, 0,  0};
    std::vector<int> incr_m  = {1, 0,  0, 0,  1, 0,  1};
    gen_code(n, m, func, args, dim, init, incr_n, incr_m);
    return 0;
}