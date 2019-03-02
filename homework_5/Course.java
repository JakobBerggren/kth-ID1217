public class Course{

    public static void main(String[] args) {
        
        Student[] students = new Student[10];

        students[0] = new Student("Arthur");
        students[1] = new Student("Becky");
        students[2] = new Student("Cesar");
        students[3] = new Student("Dave");
        students[4] = new Student("Emma");
        students[5] = new Student("Finn");
        students[6] = new Student("George");
        students[7] = new Student("Hannah");
        students[8] = new Student("Irene");
        students[9] = new Student("Jessica");


        for(int i = 0; i<students.length;i++)
            students[i].start();
    }


}