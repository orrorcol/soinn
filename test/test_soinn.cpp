#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "test.h"
#include "soinn.h"
#include "data_interface.h"
#include "data_reader.h"

using namespace std;
using namespace ann;

namespace test
{

void test() {
    string fname;
    string repr_name;
    string dataset;
    int num_data;
    int dim;
    using my_vec = my_vector<DTYPE>;
    vector<my_vec> vv;
    bool has_head = true;
    
    cout << "Choose a dataset 1(siftsmall), 2(sift), "
        "3(gist), 4(bigann), 5(fakedata):" << flush;
    int i = 0;
    if(cin >> i) {
        switch (i) {
            case 1:
                fname = "../data/siftsmall/siftsmall_base.fvecs";
                repr_name = "../data/siftsmall/siftsmall_repr.fvecs";
                dataset = "siftsmall";
                num_data = 10'000;
                dim = 128;
                break;
            case 2:
                fname = "../data/sift/sift_base.fvecs";
                repr_name = "../data/sift/sift_repr.fvecs";
                dataset = "sift";
                num_data = 1000'000;
                dim = 128;
                break;
            case 3:
                fname = "../data/gist/gist_base.fvecs";
                repr_name = "../data/gist/gist_repr.fvecs";
                dataset = "gist";
                num_data = 1000'000;
                dim = 960;
                break;
            case 4:
                fname = "../data/bigann/bigann_base.bvecs";
                repr_name = "../data/bigann/bigann_reprXM.ivecs";
                dataset = "bigann";
                num_data = 1'000'000;
                dim = 128;
                repr_name.replace(repr_name.find("XM"), 1, to_string(num_data / 1'000'000));
                break;
            case 5:
                has_head = false;
                dataset = "fakedata";
                cout << "Input fake data number(0,1,...):";
                int i;
                cin >> i;
                fname = "../data/fakedata/fake" + to_string(i);
                cout << "input data size: ";
                cin >> num_data; // 52500;
                dim = DDIM;
                break;
            default:
                cout << "Wrong input" << endl;
                return;
        }
    }
    else {
        cout << "Wrong input" << endl;
        return;
    }
    repr_name = fname + ".repr_soinn";
    repr_name += string("_") + to_string(dim);

    dreader<DTYPE>* pdreader = new dreader<DTYPE>(dim, fname, has_head);

    cout << "Reading " << fname << endl;
    while (true) {
        my_vec v(dim);
        int ct = pdreader->read_one_vec(v.get_data(), v.size());
        if (ct == 0) break;
        vv.push_back(v);
        if (vv.size() == num_data) break;
    }
    delete pdreader;
    cout << "Read " << vv.size() << " vectors" << std::endl;

    int max_layer = 1;
    ann::soinn<DTYPE> alg(max_layer, param_dead_age, param_lamda, param_c_noise);
    timer tm;
    tm.start();
    alg.learn(vv);
    tm.stop();
    double sec = tm.count() / (1e6);
    cout << "train costs " << sec << "s" << endl;
    std::cout << "Learning finished" << std::endl;
    repr_name += string("_") + to_string(sec);
    alg.save_layer(0, repr_name);
}


} // namespace test

using namespace test;

int main()
{
    //double add_c = 0.1;
    //for (param_c_noise = 0.0; param_c_noise < 1.0 + 1e-8; param_c_noise += add_c) {
    //    test::test();
    //}
    //for (param_dead_age = 10; param_dead_age < 100; ++param_dead_age) {
    //    test::test();
    //}
    //for (param_lamda = 50; param_lamda < 100; ++param_lamda) {
    //    test::test();
    //}
    cout << "Input dead age, lambda and c_noise:";
    cin >> param_dead_age >> param_lamda >> param_c_noise;
    test::test();
}
