package jp.mixi.xmpp.antistrophe;

public interface XMPPHandler {
    public void onConnect();

    public void onDisconnect();
    
    public void onError();
    
    public void onReceiveRoster(XMPPUser[] users);
    
    public void onReceiveMessage();
    
    public void onReceivePresence();
}
