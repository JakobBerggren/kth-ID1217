import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class Student{
    
    private static RMIInterface look_up;
    String myName;

    public Student(String myName){
        this.myName = myName;
    }

    public void start(){
        try{
            look_up = (RMIInterface) Naming.lookup("//localhost/Teacher");
            String partner = look_up.requestPartner(myName);
            System.out.println("my name is " + myName +  " and my partner is: " + partner);    
        } catch(Exception e){
            System.out.println("Student Exception " + myName + ": " + e.toString());
            e.printStackTrace();
        }

    }

    /*
    public static void main(String[] args) throws MalformedURLException, RemoteException, NotBoundException{

        look_up = (RMIInterface) Naming.lookup("//localhost/Teacher");
        String partner = look_up.requestPartner(myName);
        System.out.println("my partner is: " + partner);
    }
    */
}