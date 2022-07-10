package webscraper;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class Thisinh {
    private final String hoVaTen;
    private final String ngaySinh;
    private final String soBaoDanh;
    private final String diemThiCacMon;
    private float[] diemThi;

    Thisinh(String soBaoDanh) throws Exception {
        this.soBaoDanh = soBaoDanh;
        String cmd = "curl -sF SoBaoDanh=" + this.soBaoDanh + " diemthi.hcm.edu.vn/Home/Show";
        Runtime run = Runtime.getRuntime();
        Process pr = run.exec(cmd);
        pr.waitFor();
        BufferedReader buf = new BufferedReader(new InputStreamReader(pr.getInputStream()));
        String line;
        StringBuilder html = new StringBuilder();
        while ((line = buf.readLine()) != null)
            html.append(line);
        Document doc = Jsoup.parse(html.toString());
        Element element = doc.select("table tr").get(1);
        this.hoVaTen = element.select("td").get(0).text();
        this.ngaySinh = element.select("td").get(1).text();
        this.diemThiCacMon = element.select("td").get(2).text();
    }
    public String getSoBaoDanh() { return soBaoDanh; }
    public String getHoVaTen() { return hoVaTen; }
    public String getNgaySinh() { return ngaySinh; }
    public String getDiemThiCacMon() { return diemThiCacMon; }
    public void setDiemThi(ArrayList<String> monThi) {
        diemThi = new float[monThi.size()];
        byte[] indexMonThi = new byte[diemThi.length];
        for (int i=0; i<diemThi.length; ++i) {
            indexMonThi[i] = (byte)diemThiCacMon.indexOf(monThi.get(i));
            if (indexMonThi[i]!=-1)
                diemThi[i] = Float.parseFloat(diemThiCacMon.substring(
                        indexMonThi[i]+monThi.get(i).length()+2,
                        indexMonThi[i]+monThi.get(i).length()+6).split(" ")[0]
                );
            else diemThi[i] = -1;
        }
    }
    public void printInfo() {
        System.out.println("Số báo danh: "+soBaoDanh);
        System.out.println("Họ và tên: "+hoVaTen);
        System.out.println("Ngày sinh: "+ngaySinh);
        System.out.println(diemThiCacMon);
    }
    public String getInfoInCsvFormat() {
        StringBuilder result = new StringBuilder(soBaoDanh + "," + hoVaTen + "," + ngaySinh);
        for (float var : diemThi) result.append(",").append(var);
        return result.toString();
    }
}