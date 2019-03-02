import java.rmi.RemoteException;
import java.rmi.Naming;
import java.rmi.server.UnicastRemoteObject;

public class Teacher extends UnicastRemoteObject implements RMIInterface{

    private static final long serialVersionUID = 1L;

    protected Teacher() throws RemoteException{

        super();

    }

    public String requestPartner(String studentName) throws RemoteException{
        
        System.out.println("Teacher got request from: " + studentName);
        
        return studentName;
    }
    

    public static void main(String[] args) {
        
        try{
            Naming.rebind("//localhost/Teacher", new Teacher());
            System.err.println("Teacher ready");

        } catch(Exception e){
            System.out.println("Teacher exception: " + e.toString());
            e.printStackTrace();
        }
    }
}