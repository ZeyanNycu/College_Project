#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sstream>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <vector>
#include <time.h>
#include <random>
#include <vector>
#include <sys/stat.h>
#include <queue>
#include <fstream>
#include <map>
using namespace std;
bool isnum(string s);
vector <string> getcommand(string s);
string animals[] = { "aardvark", "aardwolf", "abalone", "acouchi", "addax", "adder", "adouri", "agama", "agouti", "aidi", "airedale", "akitainu", "albino", "alleycat", "alpaca", "ammonite", "amoeba", "amphiuma", "anaconda", "anchovy", "anemone", "angora", "anhinga", "ankole", "annelid", "annelida", "anole", "ant", "antbear", "anteater", "antelope", "antlion", "anura", "aoudad", "ape", "aphid", "aracari", "arachnid", "arawana", "argali", "arkshell", "armyant", "armyworm", "arrowana", "aruanas", "asp", "ass", "astarte", "auk", "auklet", "aurochs", "avians", "avocet", "axisdeer", "axolotl", "ayeaye", "aztecant", "babirusa", "baboon", "bactrian", "badger", "bagworm", "baiji", "banteng", "barb", "barbet", "barnacle", "barnowl", "basenji", "basil", "basilisk", "bass", "bat", "bats", "beagle", "bear", "beaver", "bedbug", "bee", "beetle", "bellfrog", "betafish", "bettong", "bighorn", "bilby", "bird", "bison", "bittern", "blackfly", "blacklab", "blesbok", "blobfish", "blowfish", "bluebird", "bluefish", "bluegill", "bluejay", "bluet", "boa", "boar", "bobcat", "bobolink", "bobwhite", "bongo", "bonobo", "booby", "borer", "borzoi", "boto", "boubou", "boutu", "bovine", "brant", "bream", "bronco", "buck", "budgie", "bufeo", "buffalo", "bug", "bull", "bullfrog", "bunny", "bunting", "burro", "bushbaby", "bustard", "buzzard", "caiman", "calf", "camel", "canary", "canine", "capybara", "caracal", "cardinal", "caribou", "carp", "cat", "catbird", "catfish", "cats", "cattle", "caudata", "cavy", "cero", "chafer", "chamois", "cheetah", "chick", "chicken", "chipmunk", "chital", "cicada", "cirriped", "civet", "clam", "clumber", "coati", "cob", "cobra", "cock", "cockatoo", "cod", "collie", "colt", "comet", "conch", "condor", "coney", "conure", "cony", "coot", "cooter", "copepod", "coqui", "coral", "cougar", "cow", "cowbird", "cowrie", "coyote", "coypu", "crab", "crane", "cranefly", "crayfish", "creature", "cricket", "crow", "cub", "cuckoo", "cur", "curassow", "curlew", "cuscus", "cutworm", "cygnet", "dairycow", "dartfrog", "dassie", "deer", "degu", "degus", "dikdik", "dikkops", "dingo", "dinosaur", "dipper", "discus", "dodo", "dodobird", "doe", "dog", "dogfish", "dolphin", "donkey", "dorado", "dore", "dorking", "dormouse", "dotterel", "dove", "dragon", "drake", "drever", "drongo", "duck", "duckling", "dugong", "duiker", "dunlin", "dunnart", "eagle", "earwig", "echidna", "eel", "eeve", "eft", "egg", "egret", "eider", "eland", "elephant", "elk", "elkhound", "elver", "emu", "equine", "erin", "ermine", "erne", "esok", "ewe", "eyas", "eyra", "fairyfly", "falcon", "fantail", "fanworms", "fawn", "feline", "ferret", "finch", "finwhale", "fireant", "firefly", "fish", "flamingo", "flatfish", "flea", "flee", "flicker", "flies", "flounder", "fluke", "fly", "foal", "fossa", "fowl", "fox", "foxhound", "frog", "fruitbat", "fruitfly", "fugu", "fulmar", "furseal", "gadwall", "galago", "galah", "gander", "gannet", "gar", "garpike", "gaur", "gavial", "gazelle", "gecko", "geese", "gelada", "gelding", "gemsbok", "gemsbuck", "genet", "gerbil", "gerenuk", "gharial", "gibbon", "giraffe", "glowworm", "gnat", "gnu", "goa", "goat", "godwit", "goitered", "goldfish", "gonolek", "goose", "gopher", "goral", "gorilla", "goshawk", "gosling", "gourami", "grackle", "grayfox", "grayling", "graywolf", "grebe", "grison", "grosbeak", "grouper", "grouse", "grub", "grunion", "guanaco", "gull", "guppy", "hackee", "haddock", "hagfish", "hake", "halcyon", "halibut", "halicore", "hamster", "hapuka", "hapuku", "hare", "harpseal", "harrier", "hart", "hawk", "hedgehog", "heifer", "hen", "herald", "heron", "herring", "hind", "hoatzin", "hog", "hogget", "hoiho", "hoki", "honeybee", "hoopoe", "hornbill", "hornet", "horse", "horsefly", "hound", "housefly", "hoverfly", "huemul", "huia", "human", "husky", "hydra", "hyena", "hyrax", "ibex", "ibis", "ibisbill", "icefish", "ichidna", "iggypops", "iguana", "imago", "impala", "incatern", "inchworm", "indri", "inganue", "insect", "isopod", "jabiru", "jackal", "jaeger", "jaguar", "javalina", "jay", "jenny", "jerboa", "joey", "johndory", "junco", "junebug", "kagu", "kakapo", "kangaroo", "karakul", "katydid", "kawala", "kestrel", "kid", "killdeer", "kingbird", "kinglet", "kinkajou", "kiskadee", "kite", "kitfox", "kitten", "kitty", "kiwi", "koala", "kob", "koi", "koodoo", "kouprey", "krill", "kudu", "lacewing", "ladybird", "ladybug", "lamb", "lamprey", "langur", "lark", "larva", "lcont", "leafbird", "leafwing", "leech", "lemming", "lemur", "leopard", "leveret", "lice", "liger", "limpet", "limpkin", "ling", "lion", "lionfish", "lizard", "llama", "lobo", "lobster", "locust", "longhorn", "longspur", "loon", "lorikeet", "loris", "louse", "lovebird", "lowchen", "lunamoth", "lungfish", "lynx", "lynxÂ ", "macaque", "macaw", "macropod", "maggot", "magpie", "malamute", "mallard", "mamba", "mammal", "mammoth", "manatee", "mandrill", "mangabey", "manta", "mantaray", "mantid", "mantis", "manxcat", "mara", "marabou", "mare", "marlin", "marmoset", "marmot", "marten", "martin", "mastiff", "mastodon", "mayfly", "mealworm", "meerkat", "merlin", "mice", "midge", "mink", "minnow", "mite", "moa", "mole", "mollies", "mollusk", "molly", "monarch", "mongoose", "mongrel", "monkey", "moorhen", "moose", "moray", "morayeel", "morpho", "mosasaur", "mosquito", "moth", "motmot", "mouflon", "mouse", "mudpuppy", "mule", "mullet", "muntjac", "murrelet", "muskox", "muskrat", "mussel", "mustang", "mutt", "myna", "mynah", "myotisÂ ", "nabarlek", "nag", "naga", "nagapies", "nandine", "nandoo", "nandu", "narwhal", "narwhale", "nauplius", "nautilus", "nematode", "nene", "nerka", "newt", "nightjar", "nilgai", "noctilio", "noctule", "noddy", "numbat", "nuthatch", "nutria", "nyala", "nymph", "ocelot", "octopus", "okapi", "olingo", "olm", "opossum", "orca", "oriole", "oryx", "osprey", "ostracod", "ostrich", "otter", "ovenbird", "owl", "oxen", "oxpecker", "oyster", "pacaÂ ", "panda", "pangolin", "panther", "papillon", "parakeet", "parrot", "peacock", "peafowl", "peccary", "pelican", "penguin", "perch", "pewee", "pheasant", "phoebe", "phoenix", "pig", "pigeon", "piglet", "pika", "pike", "pilchard", "pinniped", "pintail", "pipit", "piranha", "pitbull", "plankton", "platypus", "plover", "polecat", "polliwog", "polyp", "pony", "pooch", "poodle", "porpoise", "possum", "prawn", "primate", "puffer", "puffin", "pug", "pullet", "puma", "pupa", "pupfish", "puppy", "pussycat", "pygmy", "python", "quagga", "quahog", "quail", "queenant", "queenbee", "quelea", "quetzal", "quokka", "quoll", "rabbit", "raccoon", "racer", "ragfish", "rail", "ram", "raptors", "rasbora", "rat", "ratfish", "rattail", "raven", "ray", "redhead", "redpoll", "redstart", "reindeer", "reptile", "reynard", "rhea", "rhino", "ringworm", "roach", "roan", "robin", "rockrat", "rodent", "roebuck", "roller", "rook", "rooster", "sable", "saiga", "salmon", "sambar", "sardine", "sawfish", "scallop", "scarab", "scaup", "scorpion", "scoter", "screamer", "seabird", "seagull", "seahog", "seahorse", "seal", "sealion", "seaslug", "seriema", "serpent", "serval", "shark", "sheep", "shelduck", "shibainu", "shihtzu", "shoveler", "shrew", "shrike", "shrimp", "siamang", "sifaka", "silkworm", "siskin", "skimmer", "skink", "skipper", "skua", "skunk", "skylark", "sloth", "slug", "smelts", "smew", "snail", "snake", "snipe", "snowdog", "snowyowl", "songbird", "sora", "sow", "sparrow", "sphinx", "spider", "sponge", "squab", "squamata", "squeaker", "squid", "squirrel", "stag", "stallion", "starfish", "starling", "steed", "steer", "stilt", "stingray", "stinkbug", "stinkpot", "stoat", "stonefly", "stork", "stud", "sturgeon", "sunbear", "sunfish", "swallow", "swan", "swift", "tadpole", "tahr", "takin", "tamarin", "tanager", "tapaculo", "tapeworm", "tapir", "tarpan", "tarsier", "taruca", "tattler", "tayra", "teal", "tegus", "teledu", "tench", "tenrec", "termite", "tern", "terrapin", "terrier", "thrasher", "thrip", "thrush", "tick", "tiger", "tilefish", "tinamou", "titi", "titmouse", "toad", "toadfish", "tomtitÂ ", "topi", "tortoise", "toucan", "towhee", "tragopan", "trex", "trogon", "trout", "tuatara", "tuna", "turaco", "turkey", "turtle", "uakari", "umbrette", "unau", "ungulate", "unicorn", "upupa", "urchin", "urial", "urson", "urubu", "urus", "urutu", "urva", "vaquita", "veery", "verdin", "vervet", "vicuna", "viper", "vireo", "vixen", "vole", "volvox", "vulture", "wallaby", "wallaroo", "walleye", "walrus", "warbler", "warthog", "wasp", "waterbug", "watussi", "waxwing", "weasel", "weevil", "whale", "whapuku", "whelp", "whimbrel", "whippet", "whiteeye", "whooper", "widgeon", "wildcat", "willet", "wireworm", "wisent", "wolf", "wombat", "woodcock", "worm", "wrasse", "wren", "wryneck", "wyvern", "xantus", "xeme", "xenops", "xenopus", "xenurine", "xerus", "xiphias", "xoni", "xrayfish", "yaffle", "yak", "yapok", "yardant", "yearling", "yeti", "ynambu", "yucker", "zander", "zebra", "zebu", "zenaida", "zeren", "zopilote", "zorilla" };
class Channel
{
    public:
        string name;
        int num;
        string topic;
        Channel()
        {
            name = "-1";
            num = 0;
            topic = "";
        }
};
class User
{
    public:
        string name;
        string nickname;
        string channel;
        User()
        {
            name = "";
            nickname= "";
            channel="";
        }
};
void print_client(int IP,int port)
{
    int a[4];
    for(int i =0 ; i<4; i++)
    {
        a[i] = IP%256;
        IP = IP>>8;
    }
    cout << a[0] << "." << a[1] << "." << a[2] << "." << a[3] << ":" << port << endl;
}

int num = 0;
int main(int argv,char *argc[])
{
    map<string,Channel> channel;
    User user[1010];
    for(int i = 0 ; i<1010 ; i++)
    {
        user[i].name = "";
        user[i].nickname = "";
        user[i].channel = "";
    }
    socklen_t clilen;
    socklen_t peerlen;
    struct sockaddr_in cliaddr;
    struct sockaddr_in servaddr;
    struct sockaddr_in peer;
    int port = atoi(argc[1]);
    //construct the server socket
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);
    //let the socket in the state of the listen
    int a = bind(listenfd,(struct sockaddr*) &servaddr,sizeof(servaddr));
    listen(listenfd,1010);
    //initialize the fd_set
    int MAX =listenfd+1;
    int client[1010];
    struct sockaddr_in IP[1010];
    int connfd;
    fd_set rset;
    FD_ZERO(&rset);
    for(int i = 0 ; i<1010; i++) client[i]=-1;
    int cdout = dup(1);
    while(true)
    {
        //accept the connect
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(listenfd,&rset);
        MAX = listenfd+1;
        for(int i = 0 ; i<1010 ; i++)
        {
            if(client[i] == -1) continue;
            MAX = max(MAX,client[i]+1);
            FD_SET(client[i],&rset);
        }
        select(MAX,&rset,NULL,NULL,NULL);
        char buffer[100];
        for(int j = 0; j<100; j++) buffer[j]=0;
        // check if the socket is newl
        if(FD_ISSET(listenfd,&rset))
        {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*) &cliaddr,&clilen);
            if(connfd < 0)
            {
                cout << "there is the error" << endl;
                return 0;
            }
            for(int i = 0 ; i<1010; i++)
            {
                if(client[i]==-1)
                {
                    client[i] = connfd;
                    num++;
                    user[i].nickname=animals[rand()%888];
                    IP[i] = cliaddr;
                    break;
                }
            }
        }
        for(int i = 0 ; i<1010 ; i++)
        {
            if(client[i] == -1) continue;
            if(FD_ISSET(client[i],&rset))
            {
                int check = recv(client[i],buffer,100,0);
                if(check == 0)
                {
                    close(client[i]);
                    client[i] = -1;
                    if(user[i].channel != "") channel[user[i].channel].num--;
                    num--;
                    continue;
                }
                string command = buffer;
                vector <string> cmd;
                cmd = getcommand(command);
                cout << command << endl;
                //cout << command << endl;
                if(cmd[0] == "NICK")
                {
                    dup2(client[i],1);
                    if(cmd.size() == 1)
                    {
                        printf(":hw1 431 :No nickname given\r\n");
                        dup2(cdout,1);
                        continue;
                    }
                    bool sign = true;
                    for(int j = 0 ; j<1010 ; j++)
                    {
                        if(client[j] == -1 || user[j].nickname != cmd[1]) continue;
                        sign = false;
                        printf(":hw1 436 :Nickname collision KILL\r\n");
                    }
                    if(sign) user[i].nickname = cmd[1];
                    dup2(cdout,1);
                }
                else if(cmd[0] == "USERS")
                {
                    dup2(client[i],1);
                    printf(":hw1 392 :UserID   Terminal  Host\r\n");
                    for(int j = 0 ; j<1010 ; j++)
                    {
                        if(client[j] == -1) continue;
                        int port = IP[j].sin_port;
                        stringstream ss;
                        ss << port ;
                        string s = "127.0.0.1."+ss.str();
                        string w = "-";
                        printf(":hw1 393 :%-8s %-9s %-8s\r\n",user[j].nickname.c_str(),w.c_str(),s.c_str());
                    }
                    printf(":hw1 394 :End of users\r\n");
                    dup2(cdout,1);
                }
                else if(cmd[0] == "USER")
                {
                    dup2(client[i],1);
                    if(cmd.size()<5) printf(":hw1 461 %s :Not enough parameters\r\n",cmd[0].c_str());
                    else
                    {
                        int from = 0;
                        for(int j = 0 ; j<cmd[4].length() ; j++)
                        {
                            if(cmd[4][j] == ':')
                            {
                                from = j+1;
                                break;
                            }
                        }
                        user[i].name = cmd[4].substr(from,cmd[4].length()-1);
                        cout << ":hw1 001 " << user[i].nickname << " :Welcome to the minimized IRC daemon!\r\n" << endl;
                        printf(":hw1 251 :There are %d users and %d invisible on %d servers\r\n",num,0,1);
                        printf(":hw1 375 :- hw1 Message of the day - \r\n");
                        printf(":hw1 372 :- Hello, World!\r\n");
                        printf(":hw1 372 :-              @                    _ \r\n");
                        printf(":hw1 372 :-  ____  ___   _   _ _   ____.     | |\r\n");
                        printf(":hw1 372 :- /  _ `'_  \\ | | | '_/ /  __|  ___| |\r\n");
                        printf(":hw1 372 :- | | | | | | | | | |   | |    /  _  |\r\n");
                        printf(":hw1 372 :- | | | | | | | | | |   | |__  | |_| |\r\n");
                        printf(":hw1 372 :- |_| |_| |_| |_| |_|   \\____| \\___,_|\r\n");
                        printf(":hw1 372 :- minimized internet relay chat daemon\r\n");
                        printf(":hw1 376 :End of /MOTD command\r\n");
                    }
                    dup2(cdout,1);
                }
                else if(cmd[0] == "LIST")
                {
                    dup2(client[i],1);
                    printf(":hw1 321 %s Channel :Users Name\r\n",user[i].nickname.c_str());
                    for(auto it:channel)
                    {
                        if(it.second.num == 0) continue;
                        printf(":hw1 322 %s %s %d :%s\r\n",user[i].nickname.c_str(),it.second.name.c_str(),it.second.num,it.second.topic.c_str());
                    }
                    printf(":hw1 323 %s :End of Liset\r\n",user[i].nickname.c_str());
                    dup2(cdout,1);
                }
                else if(cmd[0] == "JOIN")
                {
                    dup2(client[i],1);
                    string s;
                    if(cmd.size()<2)
                    {
                        printf(":hw1 461 %s :Not enough parameters\r\n",user[i].nickname.c_str());
                        dup2(cdout,1);
                        continue;
                    }
                    else
                    {
                        printf(":%s JOIN %s\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                        s = cmd[1];
                        for(int  j = 0 ; j<1010 ; j++)
                        {
                            if(client[j] == -1 || user[j].channel != s || i == j) continue;
                            printf(":%s JOIN %s\r\n",user[j].nickname.c_str(),cmd[1].c_str());
                        }
                        if(channel.count(s) == 0) channel[s] = Channel();
                        channel[s].num++;
                        channel[s].name = s;
                        user[i].channel = s;
                        if(channel[s].topic == "")printf(":hw1 331 %s %s :No topic is set\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                        else printf(":hw1 332 %s %s :%s\r\n",user[i].nickname.c_str(),user[i].channel.c_str(),channel[s].topic.c_str());
                        printf(":hw1 353 %s %s :%s\r\n",user[i].nickname.c_str(),cmd[1].c_str(),user[i].nickname.c_str());
                        printf(":hw1 366 %s %s :End of /NAMES list\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                    }
                    dup2(cdout,1);
                    for(int  j = 0 ; j<1010 ; j++)
                    {
                        if(client[j] == -1 || user[j].channel != s || i == j) continue;
                        dup2(client[j],1);
                        printf(":%s JOIN %s\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                        dup2(cdout,1);
                    }
                }
                else if(cmd[0] == "NAMES")
                {
                    dup2(client[i],1);
                    if(cmd.size()<2)    printf(":hw1 461 %s :Not enough parameters\r\n",user[i].nickname.c_str());
                    else
                    {
                        if(user[i].channel == cmd[1])
                        {
                            printf(":hw1 353 %s %s :%s\r\n",user[i].nickname.c_str(),cmd[1].c_str(),user[i].nickname.c_str());
                        }
                        printf(":hw1 366 %s %s :End of NAMES list\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                    }
                    dup2(cdout,1);
                }
                else if(cmd[0] == "TOPIC")
                {
                    dup2(client[i],1);
                    if(cmd.size() == 2)
                    {
                        string s = cmd[1];
                        if(user[i].channel != s)
                        {
                            printf(":hw1 442 %s %s :You're not on that channel\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                            fflush(stdout);
                        }
                        else
                        {
                            if(channel[s].topic == "")printf(":hw1 331 %s %s :No topic is set\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                            else printf(":hw1 332 %s %s :%s\r\n",user[i].nickname.c_str(),user[i].channel.c_str(),channel[s].topic.c_str());
                            fflush(stdout);
                        }
                        
                    }
                    else
                    {
                        if(cmd.size()<3)
                        {
                            printf(":hw1 461 %s :Not enough parameters\r\n",user[i].nickname.c_str());
                            dup2(cdout,1);
                            continue;
                        }
                        else
                        {
                            if(user[i].channel != cmd[1])
                            {
                                printf(":hw1 442 %s %s :You're not on that channel\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                                dup2(cdout,1);
                                continue;
                            }
                            int from = 0;
                            for(int j = 0 ; j<cmd[2].length() ; j++)
                            {
                                if(cmd[2][j] == ':')
                                {
                                    from = j+1;
                                    break;
                                }
                            }
                            string s = cmd[2].substr(from,cmd[2].length()-from);
                            if(cmd.size()>2)
                            {
                                for(int j = 3; j<cmd.size() ; j++)
                                {
                                    s+=(" ");
                                    s+=(cmd[j]);
                                } 
                            }
                            printf(":hw1 332 %s %s :%s\r\n",user[i].nickname.c_str(),user[i].channel.c_str(),s.c_str());
                            channel[user[i].channel].topic = s;
                        }
                    }
                    
                    dup2(cdout,1);
                }
                else if(cmd[0] == "PART")
                {
                    dup2(client[i],1);
                    if(cmd.size()<2)
                    {
                        printf(":hw1 461 %s :Not enough parameters\r\n",user[i].nickname.c_str());
                        fflush(stdout);
                        dup2(cdout,1);
                        continue;
                    }
                    if(channel.count(cmd[1]) == 0)
                    {
                        printf(":hw1 403 %s %s :No such channel\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                        fflush(stdout);
                        dup2(cdout,1);
                        continue;
                    }
                    if(user[i].channel != cmd[1])
                    {
                        printf(":hw1 442 %s %s :You're not on that channel\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                        fflush(stdout);
                        dup2(cdout,1);
                        continue;
                    }
                    dup2(cdout,1);
                    string s = user[i].channel;
                    for(int  j = 0 ; j<1010 ; j++)
                    {
                        if(client[j] == -1 || user[j].channel != s || i == j) continue;
                        dup2(client[j],1);
                        printf(":%s PART :%s\r\n",user[i].nickname.c_str(),user[i].channel.c_str());
                        dup2(cdout,1);
                    }
                    dup2(client[i],1);
                    printf(":%s PART :%s\r\n",user[i].nickname.c_str(),user[i].channel.c_str());
                    channel[user[i].channel].num --;
                    user[i].channel = "";
                    dup2(cdout,1);
                }
                else if(cmd[0] == "PING")
                {
                    dup2(client[i],1);
                    printf("PONG localhost\r\n");
                    dup2(cdout,1);
                }
                else if(cmd[0] == "QUIT")
                {
                    close(client[i]);
                    client[i] = -1;
                    if(user[i].channel != "") channel[user[i].channel].num--;
                    num--;
                }
                else if(cmd[0] == "PRIVMSG")
                {
                    if(cmd.size()==2)
                    {
                        dup2(client[i],1);
                        printf(":hw1 412 %s :No text to send\r\n",user[i].nickname.c_str());
                        dup2(cdout,1);
                        continue;
                    }
                    if(cmd.size()==1)
                    {
                        dup2(client[i],1);
                        printf(":hw1 411 %s :No recipient given (PRIVMSG)\r\n",user[i].nickname.c_str());
                        dup2(cdout,1);
                        continue;
                    }
                    if(channel.count(cmd[1]) == 0)
                    {
                        dup2(client[i],1);
                        printf(":hw1 401 %s %s :No such channel\r\n",user[i].nickname.c_str(),cmd[1].c_str());
                        dup2(cdout,1);
                        continue;
                    }
                    bool sign = true;
                    for(int j = 0 ; j<1010 ; j++)
                    {
                        if(client[j] == -1 || user[j].channel != cmd[1] || i==j) continue;
                        sign = false;
                        int from = 0;
                        for(int k = 0 ; k<cmd[2].length() ; k++)
                        {
                            if(cmd[2][k] == ':')
                            {
                                from = k+1;
                                break;
                            }
                        }
                        string s = cmd[2].substr(from,cmd[2].length()-1);
                        for(int k = 3 ; k<cmd.size() ; k++) s+= (" "+cmd[k]);
                        dup2(client[j],1);
                        printf(":%s PRIVMSG %s :%s\r\n",user[i].nickname.c_str(),cmd[1].c_str(),s.c_str());
                        dup2(cdout,1);
                    }
                }
                else
                {
                    dup2(client[i],1);
                    cout << ":hw1 421 " << cmd[0].c_str() << " " << user[i].nickname << " :Unkown command\r\n";
                    dup2(cdout,1);
                }
            }   
        }
    }
    return 0;
}
vector <string> getcommand(string s)
{
    int pre = 0;
    vector <string> a;
    for(int i = 0;  i<s.length() ; i++)
    {
        if(s[i] == ' ' || s[i] == '\n' || s[i] == '\r')
        {
            string b = s.substr(pre,i-pre);
            if(b.length() == 0) continue;
            a.push_back(b);
            pre = i+1;
        }
    }
    return a;
}
//sprintf(buffer,"%d",num);