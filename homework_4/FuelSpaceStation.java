import java.util.*;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class FuelSpaceStation{

    Tank nitrogenTank, quantumTank;
    Dock dock;

    int nDocked;

    final Lock dockingLock = new ReentrantLock();
    final Condition notFull = dockingLock.newCondition();

    FuelSpaceStation(int v, int n, int q){
        this.dock = new Docker(v);
        this.nitrogenTank = new Tank(n);
        this.quantumTank = new Tank(q);
    }

    public void dock(){
        dock.dock();
    }
    public void undock(){
        dock.undock();
    }

    public void fillNitrogen(int n){
        nitrogenTank.fill(n);
    }
    public void fillQuantum(int n){
        quantumTank.fill(n);
    }

    public void refillNitrogen(int n){
        nitrogenTank.refill(n);
    }
    public void refillQuantum(int n){
        quantumTank.refill(n);
    }

}



private class Tank{
    
    int size;
    int currentVolume;
    
    public Tank(int size){
        this.size = size;
        this.currentVolume = 0;
    }

    synchronized void fill(int amount){
        while(currentVolume < amount)
            this.wait();
        
        currentVolume -= amount;
    }

    synchronized void refill(int amount){
        while(currentVolume + amount > size)
            this.wait();

        currentVolume += amount;
    }
}

private class Docker{

    int size;
    int currentNumber;

    public Docker(int size){
        this.size = size;
        this.currentNumber = 0;
    }

    synchronized void dock(){
        while(currentNumber == size)
            this.wait();
        
        currentNumber++;
    }

    synchronized void undock(){
        currentNumber--;
    }
}
