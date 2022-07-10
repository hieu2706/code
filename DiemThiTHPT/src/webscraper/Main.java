// This program does not work anymore

package webscraper;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Scanner;

public class Main {
    static final String fileDanhSachMonThi = "data/danhsachmonthi.txt";

    public static void main(String[] args) throws Exception {
        System.out.print("\033[H\033[2J");  
        System.out.flush();  

        ArrayList<String> monThi = new ArrayList<String>();
        try {
            BufferedReader reader = new BufferedReader(new FileReader(fileDanhSachMonThi));
            String line = reader.readLine();
            while (line!=null) {
                monThi.add(line);
                line = reader.readLine();
            }
        } catch (FileNotFoundException e) {
            throw new Exception("Không tìm thấy file "+fileDanhSachMonThi);
        }

        Scanner scanner = new Scanner(System.in);
        System.out.println("Chương trình lấy điểm thi THPT Quốc gia năm 2020\n1. Lấy dữ liệu một thí sinh.\n2. Lấy dữ liệu nhiều thí sinh.");
        byte choice=0;
        do {
            System.out.print("Nhập 1 hoặc 2: ");
            try {
                choice = scanner.nextByte();
            } catch (Exception e) {
                System.out.println("Giá trị nhập vào không hợp lệ. Vui lòng nhập 1 hoặc 2.");
                scanner.next();
            }
        } while (choice!=1&&choice!=2);
        switch (choice) {
            case 1:
                System.out.print("Mời nhập số báo danh cần tra cứu: ");
                String soBaoDanh = scanner.next();
                System.out.println("Đang tìm kiếm...\n");
                try {
                    Thisinh thisinh = new Thisinh(soBaoDanh);
                    thisinh.printInfo();
                } catch (IndexOutOfBoundsException e) {
                    System.out.println("Không tìm thấy kết quả.");
                }
                break;
            case 2:
                System.out.print("Mời nhập số lượng thí sinh: ");
                int amount = scanner.nextInt();
                System.out.print("Mời nhập tên file để lưu dữ liệu: ");
                String outputFile = scanner.next();
                FileWriter fileWriter = new FileWriter(outputFile);
                fileWriter.write("");
                if (outputFile.endsWith(".csv")) {
                    fileWriter.append("Số báo danh,Họ và tên,Ngày sinh");
                    for (String var : monThi) fileWriter.append(",").append(var);
                    fileWriter.append("\n");
                }
                System.out.println("Đang tải dữ liệu về...");
                int first = 2000001, failCount = 0;
                for (int i=first; i<first+amount; ++i) {
                    try {
                        Thisinh thisinh = new Thisinh("0" + i);
                        if (outputFile.endsWith(".csv")) {
                            thisinh.setDiemThi(monThi);
                            fileWriter.append(thisinh.getInfoInCsvFormat()).append("\n");
                        }
                        else {
                            fileWriter.append("Số báo danh: ").append(thisinh.getSoBaoDanh()).append("\n");
                            fileWriter.append("Họ và tên: ").append(thisinh.getHoVaTen()).append("\n");
                            fileWriter.append("Ngày sinh: ").append(thisinh.getNgaySinh()).append("\n");
                            fileWriter.append(thisinh.getDiemThiCacMon()).append("\n");
                            fileWriter.append("=========================\n");
                        }
                        failCount=0;
                    } catch (IndexOutOfBoundsException e) {
                        System.out.println("Không tìm thấy dữ liệu của thí sinh mang số báo danh 0"+i);
                        if (++failCount==10) break;
                    }
                }
                fileWriter.close();
                System.out.println("Dữ liệu các thí sinh đã được lưu vào file "+outputFile);
                break;
        }
    }
}
