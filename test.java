import java.io.IOException;  
import java.util.HashMap;  
import java.util.Map;  
import java.security.cert.CertificateException;  
import java.security.cert.X509Certificate;  
import javax.net.ssl.SSLContext;  
import javax.net.ssl.TrustManager;  
import javax.net.ssl.X509TrustManager;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.*;

//import sun.security.ssl.ClientHandshaker;



public class T{
      public static void main(String[] args){
          System.out.println("HelloWorld");

          try {
          int i = 10;         
          while( i -- > 0 ){
          //URL url = new URL("https://zhifu.baidu.com/pay/notify/alipaysdk?sign=dexgZOPdku%2FhoCU%2F%2BpztFsh7STL5ZZm6RI5wjlNrbifb8ItThazK00LqTqX5yPZp%2B46sHOnBjeJIM9acaGa%2F1076Lcl0q7fTD3DoVQ5CPE0Kn2wr8%2BKK5CUPut4xwDUV5YCvoRTu%2F4uGexcPGvprcie4k%2BbtF%2FDV%2BhZC8mMb7pQ%3D&sign_type=RSA&notify_data=%3Cnotify%3E%3Cpartner%3E2088511996514320%3C%2Fpartner%3E%3Cdiscount%3E0.00%3C%2Fdiscount%3E%3Cpayment_type%3E1%3C%2Fpayment_type%3E%3Csubject%3E%E4%BA%94%E6%96%97%E6%9F%B4%E7%81%AB%E9%94%85100%E4%BB%A3%E9%87%91%E5%88%B8%3C%2Fsubject%3E%3Ctrade_no%3E2016071521001004320243517122%3C%2Ftrade_no%3E%3Cbuyer_email%3E18780093544%3C%2Fbuyer_email%3E%3Cgmt_create%3E2016-07-15+22%3A08%3A59%3C%2Fgmt_create%3E%3Cquantity%3E1%3C%2Fquantity%3E%3Cout_trade_no%3E1270195393%3C%2Fout_trade_no%3E%3Cseller_id%3E2088511996514320%3C%2Fseller_id%3E%3Cout_channel_type%3EBALANCE%3C%2Fout_channel_type%3E%3Ctrade_status%3ETRADE_SUCCESS%3C%2Ftrade_status%3E%3Cis_total_fee_adjust%3EN%3C%2Fis_total_fee_adjust%3E%3Ctotal_fee%3E78.00%3C%2Ftotal_fee%3E%3Cgmt_payment%3E2016-07-15+22%3A09%3A00%3C%2Fgmt_payment%3E%3Cseller_email%3Epay.mpaynuomi%40baidu.com%3C%2Fseller_email%3E%3Cprice%3E78.00%3C%2Fprice%3E%3Cbuyer_id%3E2088802596627320%3C%2Fbuyer_id%3E%3Cout_channel_amount%3E78.00%3C%2Fout_channel_amount%3E%3Cuse_coupon%3EN%3C%2Fuse_coupon%3E%3C%2Fnotify%3E");
          URL url = new URL("https://zhifu.baidu.com/pay/notify/alipaysdk?sign=dexgZOPdku%2FhoCU%2F%2BpztFsh7STL5ZZm6RI5wjlNrbifb8ItThazK00LqTqX5yPZp%2B46sHOnBjeJIM9acaGa%2F1076Lcl0q7fTD3DoVQ5CPE0Kn2wr8%2BKK5CUPut4xwDUV5YCvoRTu%2F4uGexcPGvprcie4k%2BbtF%2FDV%2BhZC8mMb7pQ%3D&sign_type=RSA&notify_data=%3Cnotify%3E%3Cpartner%3E2088511996514320%3C%2Fpartner%3E%3Cdiscount%3E0.00%3C%2Fdiscount%3E%3Cpayment_type%3E1%3C%2Fpayment_type%3E%3Csubject%3E%E4%BA%94%E6%96%97%E6%9F%B4%E7%81%AB%E9%94%85100%E4%BB%A3%E9%87%91%E5%88%B8%3C%2Fsubject%3E%3Ctrade_no%3E2016071521001004320243517122%3C%2Ftrade_no%3E%3Cbuyer_email%3E18780093544%3C%2Fbuyer_email%3E%3Cgmt_create%3E2016-07-15+22%3A08%3A59%3C%2Fgmt_create%3E%3Cquantity%3E1%3C%2Fquantity%3E%3Cout_trade_no%3E1270195393%3C%2Fout_trade_no%3E%3Cseller_id%3E2088511996514320%3C%2Fseller_id%3E%3Cout_channel_type%3EBALANCE%3C%2Fout_channel_type%3E%3Ctrade_status%3ETRADE_SUCCESS%3C%2Ftrade_status%3E%3Cis_total_fee_adjust%3EN%3C%2Fis_total_fee_adjust%3E%3Ctotal_fee%3E78.00%3C%2Ftotal_fee%3E%3Cgmt_payment%3E2016-07-15+22%3A09%3A00%3C%2Fgmt_payment%3E%3Cseller_email%3Epay.mpaynuomi%40baidu.com%3C%2Fseller_email%3E%3Cprice%3E78.00%3C%2Fprice%3E%3Cbuyer_id%3E2088802596627320%3C%2Fbuyer_id%3E%3Cout_channel_amount%3E78.00%3C%2Fout_channel_amount%3E%3Cuse_coupon%3EN%3C%2Fuse_coupon%3E%3C%2Fnotify%3E");
          HttpURLConnection connection = (HttpURLConnection) url.openConnection();
          connection.setUseCaches(false);
          BufferedReader br = new BufferedReader(new InputStreamReader(
        		  connection.getInputStream()
        		  ));
          String line;
          StringBuilder sb = new StringBuilder();
          while ((line = br.readLine()) != null) {// ѭ����ȡ��
                sb.append(line);
          }
          br.close();// �ر���
          connection.disconnect();// �Ͽ�����
          System.out.println(sb.toString());
            Thread.sleep(1000*2);
          }

           } catch (Exception e) {
               e.printStackTrace();
               System.out.println("ʧ��!");
           }
      }
}
