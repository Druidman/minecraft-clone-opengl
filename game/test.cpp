// #include <vector>
// #include <chrono>
// #include <iostream>
// class Foo {
//     public:
//         int count;
//         std::vector<float> nums;
//         Foo(int count) : count(count), nums(std::vector<float>(count, 1.0f)){};
// };
// bool testData(std::vector<float> *data){
//     for (int i=0; i < data->size(); i++){
//         if (data->at(i) != 1.0f){
//             return false;
//         }
//     }
//     return true;
// }

// void testCopy(){
//     std::vector<float> data;
//     std::vector<Foo> sources(1024, Foo(3000));
//     // compute size
//     std::cout << "\n\nCOPYING_TEST\n\n";
    
//     size_t size = 0;
//     for (Foo &foo : sources){
//         size += foo.nums.size();
//     }

//     data.resize(size, 0.0f);


//     auto start = std::chrono::high_resolution_clock::now();


//     float* dest = data.data();
//     for (Foo &foo : sources){
//         dest = std::copy(foo.nums.data(), foo.nums.data() + foo.nums.size(), dest);
//     }


//     auto end = std::chrono::high_resolution_clock::now();



//     if (!testData(&data)){
//         std::cout << "Copying memory didn't pass..\n";
//     }
//     std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

//     std::cout << "Copying memory time: " << duration << "\n";
// }
// void testGen(){
//     std::vector<float> data;
//     std::vector<Foo> sources(1024, Foo(3000));
//     // compute size
//     size_t size = 0;
//     for (Foo &foo : sources){
//         size += foo.nums.size();
//     }

//     data.resize(size, 0.0f);

//     std::cout << "\n\nGEN_TEST\n\n";


//     auto start = std::chrono::high_resolution_clock::now();


//     for (Foo &foo : sources){
//         for (int i = 0; i < foo.count; i++){
//             data[i] = 1.0f;
//         }
//     }


//     auto end = std::chrono::high_resolution_clock::now();

//     if (!testData(&data)){
//         std::cout << "Generating didn't pass..\n";
//     }
//     std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

//     std::cout << "Generating time: " << duration << "\n";
// }


// int main() {

//     // copy test
//     testCopy();
//     testGen();

//     return 0;
// }