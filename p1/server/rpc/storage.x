struct query_msg {
    string msg<255>;
    string md5<32>;
};

program MSG_STORE {
    version MSG_STORE_V {
        int INIT() = 1;
        int INSERT(string sender<255>, string receiver<255>, string id<255>,
            string msg<255>, string md5<32>) = 2;
        int MSG_N(string sender<255>) = 3;
        query_msg QUERY(string user<255>, string id<255>) = 4;
    } = 1;
} = 99;
