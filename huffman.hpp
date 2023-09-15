#include<vector>
#include<cstring>
#include<queue>
#include<cstdio>

template<typename CodeType=char>
class HuffmanTree {
typedef CodeType T;
public:
    HuffmanTree():root(nullptr) {}

    template<typename CountType=unsigned>
    std::vector<std::pair<T,std::string> > transcode
        (std::vector<std::pair<T,CountType> > &count) {
        build(count);
        std::vector<std::pair<T,std::string> > res;
        res.reserve(count.size());
        std::string code;
        code.reserve(count.size());
        transcode_base(root,code,res);
        clear(root);
        root=nullptr;
        return res;
    }

private:
    struct Node {
        T key;
        Node *lson;
        Node *rson;
        Node():key(),lson(nullptr),rson(nullptr) {}
        Node(T _key):key(_key),lson(nullptr),rson(nullptr) {}
    };
    template<typename CountType>
    struct IntNode {
        CountType val;
        Node *node;
        IntNode(CountType _val,Node* _node):val(_val),node(_node) {}
        bool operator<(const IntNode &tar) const {
            return val>tar.val;
        }
    };
    Node * root;

    template<typename CountType>
    void build(std::vector<std::pair<T,CountType> > &count) {
        std::priority_queue<IntNode<CountType> > PQ;
        for(auto p:count) {
            PQ.push(IntNode<CountType>(p.second,new Node(p.first)));
        }
        while(PQ.size()>1) {
            auto p=PQ.top();
            PQ.pop();
            auto q=PQ.top();
            PQ.pop();
            Node *wn=new Node();
            wn->lson=p.node;
            wn->rson=q.node;
            PQ.push(decltype(p)(p.val+q.val,wn));
        }
        root=PQ.top().node;
    }

    void transcode_base(Node *root_now,std::string &code_now,
        std::vector<std::pair<T,std::string> > &dst) {

        if(root_now->lson==nullptr&&root_now->rson==nullptr) {
            dst.emplace_back(root_now->key,code_now);
            return ;
        }
        if(root_now->lson!=nullptr) {
            code_now.push_back('0');
            transcode_base(root_now->lson,code_now,dst);
            code_now.pop_back();
        }
        if(root_now->rson!=nullptr) {
            code_now.push_back('1');
            transcode_base(root_now->rson,code_now,dst);
            code_now.pop_back();
        }
    }

    void clear(Node* root_now) {
        if(root_now->lson!=nullptr) clear(root_now->lson);
        if(root_now->rson!=nullptr) clear(root_now->rson);
        delete root_now;
    }
};

template<typename CodeType>
class DecodingTree {

typedef CodeType T;

public:
    DecodingTree() {
        root=new Node();
        nw=root;
    }
    
    void insert(T v,std::string s) {
        Node *now=root;
        for(auto i:s) {
            if(i=='0') {
                if(now->lson==nullptr) now->lson=new Node();
                now=now->lson;
            }
            else {
                if(now->rson==nullptr) now->rson=new Node();
                now=now->rson;
            }
        }
        now->tag=true,now->val=v;
    }
    
    std::pair<bool,char> next_bit(bool bit) {
        if(bit) {
            if(nw->rson!=nullptr) nw=nw->rson;
            else nw=root->rson;
        }
        else {
            if(nw->lson!=nullptr) nw=nw->lson;
            else nw=root->lson;
        }
        return std::pair<bool,char>(nw->tag,nw->val);
    }

    ~DecodingTree() {clear(root);}
private:
    struct Node {
        Node *lson,*rson;
        bool tag;
        T val;
        Node():lson(nullptr),rson(nullptr),tag(),val() {}
    };

    Node *root,*nw;

    void clear(Node* root_now) {
        if(root_now->lson!=nullptr) clear(root_now->lson);
        if(root_now->rson!=nullptr) clear(root_now->rson);
        delete root_now;
    }
};