package jp.mixi.xmpp.antistrophe;

public class XMPPUser {
    private String mName;
    private String mJid;
    
    public void setName(String name) {
        mName = name;
    }
    
    public String getName() {
        return mName;
    }
    
    public void setJid(String jid) {
        mJid = jid;
    }
    
    public String getJid() {
        return mJid;
    }
}
