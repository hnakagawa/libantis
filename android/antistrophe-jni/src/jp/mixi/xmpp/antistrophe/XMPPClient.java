package jp.mixi.xmpp.antistrophe;

public class XMPPClient {
    private int mNative;

    private String mHost;

    private String mJid;

    private String mPass;

    private XMPPHandler mXMPPHandler;

    private static boolean sInitialized;

    static {
        System.loadLibrary("antistrophe-jni");
    }

    public enum Presence {
        CHAT(0), AWAY(1), XA(2), DND(3);

        private int mValue;

        Presence(final int value) {
            mValue = value;
        }

        public int getValue() {
            return mValue;
        }
    }

    public static XMPPClient newInstance(String host, String jid, String pass) {
        if (!sInitialized) {
            synchronized (XMPPClient.class) {
                if (!sInitialized) {
                    XMPPClient.initialize();
                    sInitialized = true;
                }
            }
        }
        return new XMPPClient(host, jid, pass);
    }

    private XMPPClient(String host, String jid, String pass) {
        mHost = host;
        mJid = jid;
        mPass = pass;
    }

    public void connect(XMPPHandler handler) {
        mXMPPHandler = handler;
        connect(mHost, mJid, mPass, true);
    }

    protected void onConnect() {
        mXMPPHandler.onConnect();
    }

    protected void onDisconnect() {
        mXMPPHandler.onDisconnect();
    }

    protected void onError() {
        mXMPPHandler.onError();
    }

    protected void onReceiveMessage() {
        mXMPPHandler.onReceiveMessage();
    }

    protected void onReceivePresence() {
        mXMPPHandler.onReceivePresence();
    }

    protected void onReceiveRoster(String[] names, String[] jids) {
        XMPPUser[] users = new XMPPUser[names.length];
        for (int i = 0; i < names.length; ++i) {
            users[i].setName(names[i]);
            users[i].setJid(jids[i]);
        }
        
        mXMPPHandler.onReceiveRoster(users);
    }
    
    public void sendPrecense(Presence presence, String status) {
        sendPrecense(presence.getValue(), status);
    }
    
    public native void sendMessage(String msg, String to);

    public native void sendRoster();

    public native void sendRawString(String str);
    
    private native void sendPrecense(int show, String status);

    public native void close();

    private native int connect(String host, String jid, String pass, boolean debug);

    private static native void initialize();

    private static native void destroy();
}
