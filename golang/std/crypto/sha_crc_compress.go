package crypto

import (
	"bytes"
	"compress/gzip"
	"compress/zlib"
	"crypto/sha256"
	"crypto/sha512"
	"fmt"
	"hash/crc32"
	"time"

	//"github.com/cyberdelia/lzo"
	"github.com/golang/snappy"
)

/*
在64位的RHEL 8.6VM上测试，对不同大小的内存数据分别计算SHA256、SHA512（SHA512针对64位系统做过特别设计优化）以及CRC32：
=== RUN   TestShaCrcLzo

Generating data of size [16 Bytes].
SHA256 sum:[6febafe1], elapsed time: [    7.722] micro seconds.
SHA512 sum:[6e5c4859], elapsed time: [    4.240] micro seconds.
CRC32  sum:[3188ff56], elapsed time: [   18.163] micro seconds.

Generating data of size [32 Bytes].
SHA256 sum:[e8b4ac2e], elapsed time: [    0.672] micro seconds.
SHA512 sum:[738a5d86], elapsed time: [    0.704] micro seconds.
CRC32  sum:[f7889e65], elapsed time: [    0.154] micro seconds.

Generating data of size [64 Bytes].
SHA256 sum:[ab060e29], elapsed time: [    1.002] micro seconds.
SHA512 sum:[4428e58e], elapsed time: [    0.663] micro seconds.
CRC32  sum:[7708df25], elapsed time: [    0.181] micro seconds.

Generating data of size [128 Bytes].
SHA256 sum:[3da63888], elapsed time: [    1.323] micro seconds.
SHA512 sum:[66f6fac2], elapsed time: [    1.126] micro seconds.
CRC32  sum:[8d10471d], elapsed time: [    0.194] micro seconds.

Generating data of size [256 Bytes].
SHA256 sum:[37c41507], elapsed time: [    2.123] micro seconds.
SHA512 sum:[f693ecb9], elapsed time: [    1.570] micro seconds.
CRC32  sum:[a799e8c4], elapsed time: [    0.108] micro seconds.

Generating data of size [512 Bytes].
SHA256 sum:[6a50b084], elapsed time: [    3.706] micro seconds.
SHA512 sum:[3e3a7a18], elapsed time: [    2.452] micro seconds.
CRC32  sum:[a159588a], elapsed time: [    0.118] micro seconds.

Generating data of size [1 KB].
SHA256 sum:[2a147017], elapsed time: [    6.730] micro seconds.
SHA512 sum:[9c1647cd], elapsed time: [    4.193] micro seconds.
CRC32  sum:[50b6f786], elapsed time: [    0.148] micro seconds.

Generating data of size [32 KB].
SHA256 sum:[c1168143], elapsed time: [  193.125] micro seconds.
SHA512 sum:[84a7d474], elapsed time: [  111.099] micro seconds.
CRC32  sum:[57f66d71], elapsed time: [    2.454] micro seconds.

Generating data of size [64 KB].
SHA256 sum:[5e387a0a], elapsed time: [  387.062] micro seconds.
SHA512 sum:[7ded2b7a], elapsed time: [  221.269] micro seconds.
CRC32  sum:[077efcb9], elapsed time: [    4.773] micro seconds.

Generating data of size [128 KB].
SHA256 sum:[93304c18], elapsed time: [  779.298] micro seconds.
SHA512 sum:[28b5bd42], elapsed time: [  441.924] micro seconds.
CRC32  sum:[77b302c5], elapsed time: [    7.298] micro seconds.

Generating data of size [256 KB].
SHA256 sum:[1af65cbf], elapsed time: [ 1553.678] micro seconds.
SHA512 sum:[f5ca8bd5], elapsed time: [  884.683] micro seconds.
CRC32  sum:[f20ba28b], elapsed time: [   16.038] micro seconds.
--- PASS: TestShaCrcLzo (0.02s)
因为没有做严格的基准测试，只是跑了一遍测试用例，所以得出的是非常粗狂的结论：
	1. 在64位系统，对于默认的设置，完全相同的数据，SHA512的性能明显好于SHA256，并且随着数据块大小翻倍，计算时间也在翻倍
		128K数据块计算SHA512需要不到0.5毫秒
	2. CRC32的计算时间没有随着数据块的变大出现明显的增长，虽然后面也在成倍增长，但是128KB的数据块也只需要不到8微秒,
		同样的数据块，CRC32的计算时间大概只有SHA512的几十分之一。
	3. 压缩所需时间要比SHA512大很多, TODO: 完善压缩验证

测试VM配置规格：8个下面的CPU + 12GB的内存：
# cat /proc/cpuinfo
processor       : 0
vendor_id       : GenuineIntel
cpu family      : 6
model           : 45
model name      : Intel(R) Xeon(R) Gold 5218 CPU @ 2.30GHz
stepping        : 2
microcode       : 0x5003302
cpu MHz         : 2294.609
cache size      : 22528 KB
physical id     : 0
siblings        : 2
core id         : 0
cpu cores       : 2
apicid          : 0
initial apicid  : 0
fpu             : yes
fpu_exception   : yes
cpuid level     : 13
wp              : yes
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss ht syscall nx rdtscp lm constant_tsc arch_perfmon nopl xtopology tsc_reliable nonstop_tsc cpuid pni pclmulqdq ssse3 cx16 pcid sse4_1 sse4_2 x2apic popcnt tsc_deadline_timer aes xsave avx hypervisor lahf_lm pti ssbd ibrs ibpb stibp tsc_adjust arat md_clear flush_l1d arch_capabilities
bugs            : cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass l1tf mds swapgs itlb_multihit
bogomips        : 4589.21
clflush size    : 64
cache_alignment : 64
address sizes   : 45 bits physical, 48 bits virtual
power management:
*/

var KB = 1024
var MB = 1024 * 1024
var sizes = []int{16, 32, 64, 128, 256, 512, KB, 32 * KB, 64 * KB, 128 * KB, 256 * KB}

func sha256_sha512_crc32_lzo_with_diff_block_size() {
	for i := 0; i < len(sizes); i++ {
		data := generate_data_with_size(sizes[i])
		sha256_sum(data)
		sha512_sum(data)
		crc32_sum(data)
		//lzo_compress(data)
		//gzip_compress(data)
		//zlib_compress(data)
		//snappy_compress(data)
	}
}

func sha256_sum(data []byte) {
	h := sha256.New()
	start := time.Now()
	h.Write(data)
	sum := h.Sum(nil)
	end := time.Now()
	fmt.Printf("SHA256 sum:[%08x], elapsed time: [%9.3f] micro seconds.\n",
		sum[0:4], float64(end.Sub(start))/1000)
}

func sha512_sum(data []byte) {
	h := sha512.New()
	start := time.Now()
	h.Write(data)
	sum := h.Sum(nil)
	end := time.Now()
	fmt.Printf("SHA512 sum:[%08x], elapsed time: [%9.3f] micro seconds.\n",
		sum[0:4], float64(end.Sub(start))/1000)
}

func crc32_sum(data []byte) {
	start := time.Now()
	//除了IEEE, Golang一共定义了3种多项式常量，参考https://pkg.go.dev/hash/crc32@go1.21.6
	crc32_sum := crc32.ChecksumIEEE(data)
	end := time.Now()

	fmt.Printf("CRC32  sum:[%08x], elapsed time: [%9.3f] micro seconds.\n",
		crc32_sum, float64(end.Sub(start))/1000)

}

// TODO: Don't found liblzo2-dev rpm for rhel8.6 and compilation failed due to following error
//# github.com/cyberdelia/lzo
///root/go/pkg/mod/github.com/cyberdelia/lzo@v1.0.0/lzo.go:5:10: fatal error: lzo/lzo1x.h: No such file or directory
// #include <lzo/lzo1x.h>
//          ^~~~~~~~~~~~~
//compilation terminated.
/*
func lzo_compress(data []byte) error {
	buf := new(bytes.Buffer)

	start := time.Now()
	w, err := lzo.NewWriterLevel(buf, lzo.BestSpeed)
	if err != nil {
		fmt.Println("[ERROR] Cannot initialize LZO writer." + err.Error())
		return err
	}

	_, err = w.Write(data)
	if err != nil {
		fmt.Println("[ERROR] Cannot compress data with LZO mode." + err.Error())
		return err
	}
	end := time.Now()

	compres_data := buf.Bytes()
	fmt.Printf("LZO compressed size:[%s], elapsed time: [%9.3f] micro seconds.\n\n",
		convert_blcok_size(len(compres_data)), float64(end.Sub(start))/1000)
	return nil
}
*/

func gzip_compress(data []byte) error {
	buf := new(bytes.Buffer)

	start := time.Now()
	w, err := gzip.NewWriterLevel(buf, gzip.BestSpeed)
	if err != nil {
		fmt.Println("[ERROR] Cannot initialize gzip writer." + err.Error())
		return err
	}

	_, err = w.Write(data)
	if err != nil {
		fmt.Println("[ERROR] Cannot compress data with gzip mode." + err.Error())
		return err
	}
	end := time.Now()

	compres_data := buf.Bytes()
	fmt.Printf("gzip compressed:[%s], elapsed time: [%9.3f] micro seconds.\n",
		convert_blcok_size(len(compres_data)), float64(end.Sub(start))/1000)
	return nil
}

func zlib_compress(data []byte) error {
	buf := new(bytes.Buffer)

	start := time.Now()
	w, err := zlib.NewWriterLevel(buf, zlib.BestSpeed)
	if err != nil {
		fmt.Println("[ERROR] Cannot initialize zlib writer." + err.Error())
		return err
	}

	_, err = w.Write(data)
	if err != nil {
		fmt.Println("[ERROR] Cannot compress data with zlib mode." + err.Error())
		return err
	}
	end := time.Now()

	compres_data := buf.Bytes()
	fmt.Printf("zlib compressed:[%s], elapsed time: [%9.3f] micro seconds.\n",
		convert_blcok_size(len(compres_data)), float64(end.Sub(start))/1000)
	return nil
}

func snappy_compress(data []byte) error {
	buf := new(bytes.Buffer)

	start := time.Now()
	w := snappy.NewBufferedWriter(buf)
	_, err := w.Write(data)
	if err != nil {
		fmt.Println("[ERROR] Cannot compress data with snappy mode." + err.Error())
		return err
	}
	end := time.Now()

	compres_data := buf.Bytes()
	fmt.Printf("snappy compressed:[%s], elapsed time: [%9.3f] micro seconds.\n",
		convert_blcok_size(len(compres_data)), float64(end.Sub(start))/1000)
	return nil
}
