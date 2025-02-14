Bu proje, Qt kullanarak geliştirilen bir Bluetooth bağlantı uygulamasıdır. Uygulama sayesinde Bluetooth cihazlarını tarayabilir, bağlanabilir ve bağlantıyı kesebilirsiniz.
Özellikler

    Bluetooth cihazlarını tara 
    Cihaza bağlan 
    Bağlantıyı kes 
    Bağlantı durumunu göster 
    Log ekranında bağlantı geçmişini gör 

Kurulum

    Projeyi indir:

git clone https://github.com/kuvvetliisik/mte_ble_agent.git
cd mte_ble_agent

Qt ve bağımlılıkları yükle (Linux için):

sudo apt update
sudo apt install qt6-base-dev qt6-connectivity-dev

Projeyi derleyip çalıştır:

    mkdir build
    cd build
    cmake ..
    make
    ./mte_ble_agent

Kullanım

    "Cihazları Tara" butonuna basarak Bluetooth cihazlarını listeleyin.
    Bağlanmak için cihazı seçip "Bağlan" butonuna basın.
    Bağlantıyı kesmek için "Bağlantıyı Kes" butonuna basın.
    Log ekranında işlem geçmişinizi takip edin.

Notlar

    Bağlanmadan önce cihazın Bluetooth’unun açık olduğundan emin olun.
    Bağlantı başarısız olursa, cihazı tekrar tarayıp yeniden bağlanmayı deneyin.
