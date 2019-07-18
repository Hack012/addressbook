#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT addressbook: public contract {
    public:

    using contract::contract;

       ACTION upsert(name user, std::string first_name, std::string last_name, uint32_t age) {
           require_auth(user);

           address_index forUpsert(get_self(), get_self().value);
           auto itr = forUpsert.find(user.value);

           if(itr == forUpsert.end()){ //table의 정보 생성
           forUpsert.emplace(user, [&](auto& row){
               row.user = user;
               row.first_name = first_name;
               row.last_name = last_name;
               row.age = age;
           });
           
           }
           else {
               forUpsert.modify(itr, user, [&]( auto& row){ // 정보가 있을경우(수정)
                   row.user = user;
                   row.first_name = first_name;
                   row.last_name = last_name;
                   row.age = age;
               });
           }
           
           print("!!!upsert success!!!");
       } //upsert: 내가 정보를 저장할 때, 사용자의 유무를 파악해서, 내가 추가하고자 하는 정보가 존재하지 않는다면, 새로 추가해주는 ACTION이다.(수정이 가능하다)
       ACTION insert(name user, std::string first_name, std::string last_name, uint32_t age) { //push action 할 때 이 정보들 모두를 입력해줘야 한다.
           require_auth(user);//다른사람이 실행하지 못하도록 막음

           address_index forInsert(get_self(), get_self().value);
           auto itr = forInsert.find(user.value); //auto: 뒤에 입력하는 자료형에 따라서 자동으로 형식이 변하기 때문에 auto이다.(자료형의 일종)

           check(itr == forInsert.end(), "already exists");//만약 end가 없다면, 이미 정보가 있다고 오류메세지 출력

           forInsert.emplace(user, [&](auto& row){ //row: 테이블 객체
               row.user = user;
               row.first_name = first_name;
               row.last_name = last_name;
               row.age = age;
           });

           print("insert success");
       }
       ACTION erase(name user)/*erase 파라미터가 들어가는 공간 */ {
           require_auth(user);

           address_index forErase(get_self(), get_self().value);//지우기 위한 테이블이라는 뜻, forErase의 출처가 될 contract: get_self(컨트렉트 배포한 사람의 테이블에 접근하기 위함)
           auto itr = forErase.require_find(user.value, "no account"); //itr이라는 변수에 forErase의 정보를 찾아서 넣는다.
           forErase.erase(itr);

           print("erase success");
       } //지우기
    private:
    struct [[eosio::table]] person {
        name user; //한 사람의 계정 정보(Table person)
        std::string first_name;
        std::string last_name;
        uint32_t age;

        uint64_t primary_key() const { return user.value; }//primary_key() 정의, value(매개변수)
    }; //TABLE이름은 나중에 선언할 수 있다, person은 구조체 이름이다.
    //primary_key()는 이름을 바꾸면 안됨

    typedef multi_index<"people"_n, person> address_index;//"people"_n이 테이블 이름, address_index를 호출해서 테이블을 수정하겠다.

};