#pragma once

// 1. Project Headers
#include "Glyph.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

// 6. C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>

// Declaration
namespace N503::Renderer2D::Canvas::Font
{

    class Atlas
    {
    public:
        // ASCII 基本ラテン文字 (U+0020–U+007E)
        static constexpr std::u32string_view BasicLatin = U" !\"#$%&'()*+,-./0123456789:;<=>?"
                                                          U"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                                          U"`abcdefghijklmnopqrstuvwxyz{|}~";

        // ひらがな (U+3041–U+3096) + 濁点・半濁点・繰り返し記号
        static constexpr std::u32string_view Hiragana = U"ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞ"
                                                        U"ただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽ"
                                                        U"まみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ";

        // カタカナ (U+30A1–U+30F6) + 長音符
        static constexpr std::u32string_view Katakana = U"ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾ"
                                                        U"タダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポ"
                                                        U"マミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶ"
                                                        U"ーヽヾ";

        // 日本語記号・句読点
        static constexpr std::u32string_view JapaneseSymbols = U"　、。・「」『』【】〔〕〈〉《》…―～";

        // 常用漢字 2136 字 (2010年内閣告示)
        static constexpr std::u32string_view
            JoyoKanji = U"亜哀挨愛曖悪握圧扱宛嵐安暗以衣位囲医依委威為畏胃尉異移萎偉椅彙意違維慰遺緯域育壱逸茨芋引印因咽姻員院淫陰飲隠韻右宇羽雨唄鬱畝浦運雲"
                        U"永泳英映栄営詠鋭液疫益駅悦越謁閲円延沿炎垣宴援遠鉛塩演縁艶汚王凹央応往押旺欧殴桜翁奥横岡屋億憶臆虞乙俺卸音恩温穏"
                        U"下化火仮何花佳価果河苛科架夏家荷華菓貨渦過嫁暇禍靴寡歌箇稼課蚊牙瓦我画芽賀雅餓介回灰会快戒改怪拐悔海界皆械絵開階塊楷解潰壊懐諧"
                        U"貝外劾害崖概涯碍蓋街該骸各角拡革格核殻郭覚較隔閣確獲嚇穫学岳楽額顎掛潟括活喝渇割葛滑褐轄且株釜鎌刈干刊甘汗缶完肝官冠巻看陥乾勘患"
                        U"貫寒喚堪換敢棺款間閑勧寛幹感漢慣管関歓監緩憾還館環簡観韓艦鑑丸含岸眼岩頑顔願"
                        U"企伎危机気岐希忌汽奇祈季紀規記起軌飢騎鬼帰基寄亀喜幾揮期棋貴棄毀旗器畿機技宜偽欺義疑儀戯犠菊吉喫詰却脚虐及逆丘久仇休吸宮弓急救"
                        U"朽求泣球究窮給旧牛去巨居拒拠挙虚許距魚御漁凶共叫狂京享供協況峡挟強教郷境橋矯鏡競響驚仰暁業凝曲極玉均斤近金菌勤琴筋僅禁緊錦謹吟"
                        U"九句区苦駆具愚空偶遇隅串屈掘窟熊繰君訓勲薫軍郡群"
                        U"兄刑形系径茎係型契計恵啓掲渓経蛍敬景軽傾携継詣慶憬稽穴血決結建研県肩見憲権懸験顕元幻玄言限原現舷減源厳"
                        U"己古戸固股孤弧虎故枯個庫湖雇誇鼓顧五互午呉後娯悟碁語誤護口工公勾孔功巧広甲交光向好江考行坑孝抗攻更効幸拘肯侯厚恒洪皇紅荒郊香候"
                        U"校耕航貢降高康控黄慌港硬喉項溝閥構稿綱酵鉱剛傲豪克告谷刻国黒穀酷獄骨困婚恨根混紺魂今昆"
                        U"才采砕宰栽彩採済祭斎細菜最裁債催塞歳載際崎削索搾策酢錯咲冊擦刷察撮雑皿三山惨産傘散賛残斬暫"
                        U"使仕史司四市矢旨仔支伺刺姉始姿思指師枝施肢脂紙至視詩試誌諮資飼歯事似侍児士寺時滋慈辞磁示字"
                        U"鹿式識軸七疾執失嫉室悉湿漆質実芝社車遮蛇邪借尺爵酌釈寂朱殊珠砂手狩首儒寿受呪授需囚収州修周宗拾秀舟終就従柔集醜住重渋銃獣縦"
                        U"叔祝宿淑粛塾熟俊春盾巡准循順処初所書庶昇松渉尚消焼訟章笑粧紹唱情清晴象証詔奨称乗脂諸除食触植殖色辱"
                        U"伸信神申森審刃壬仁尽迅甚陣腎図推随数髄崇枢世制勢姓征性成政整星晴盛精製誠請逝醒青静税"
                        U"石績責赤切折拙窃設雪接摂節説舌銭漸浅線選煎鮮前善壮早相争走奏草創装喪葬総造僧像増憎臓蔵贈促即俗賊続卒率尊損村遜"
                        U"他多太汰唾堕惰駄体対貸袋待怠態滞戴腿替泰台宅択沢拓卓諾濯達脱奪棚谷断暖弾段男談"
                        U"知地池痴稚置恥致遅嗜竹畜蓄逐秩窒茶嫡着中仲宙忠抽昼柱注虫衷鋳駐著貯丁弔庁兆町長挑帳張彫眺釣聴超跳懲潮直勅沈珍賃鎮陳"
                        U"追椎通痛塚漬坪亭庭廷弟定底抵邸泥的転展貼摘滴適哲徹迭典天田店添転殿冬刀土唐努塗奴怒到倒党島投凍搭盗渡塔登道銅峠得徳特督篤毒独読"
                        U"栃凸突届豚鈍那内軟難二尼弐肉日乳入如妊忍認任年念燃粘乃悩農濃"
                        U"把覇婆廃拝排敗買賠薄迫漠爆縛肌鉢髪伐罰抜閥反半犯帆搬斑板氾汎伴盤番般販煩頒範繁藩晩蛮"
                        U"卑否批彼被比皮避尾微匹必泌筆姫百表評漂武不付夫婦符富布封府普浮負怖附侮撫舞譜賦赴訃腐敷膚"
                        U"復副複福服覆払沸噴焚文分吻雰墳憤紛奮並米片変偏編便勉"
                        U"保補歩穂募墓某棒冒防望傍帽暴泡縫砲包忘坊邦奉宝蜂崩剖倣法膨傍謀頰北木牧朴睦僕墨没翻本凡盆"
                        U"摩磨魔麻埋妹昧枚毎幕骸膜又末抹満慢漫魅未味密脈妙民眠矛務無夢霧娘"
                        U"名命明盟鳴迷銘滅免面綿茂模毛妄盲耗猛網目黙門問紋"
                        U"夜野矢厄役薬訳躍闇由油癒諭輸唯勇幽有夕優佑郵遊裕誘憂融与予余誉預幼容庸揚揺溶謡陽養慾抑欲翼"
                        U"来裸羅頼雷絡酪欄覧裏裂礼列廉練連錬炉老録麓論和話賄湾腕";

        // 全文字セット (BasicLatin + 日本語)
        // Device::CreateAtlas() に渡す際は BuildFullCharset() で結合して使う
        static auto BuildFullCharset() -> std::u32string
        {
            std::u32string result;
            result.reserve(BasicLatin.size() + Hiragana.size() + Katakana.size() + JapaneseSymbols.size() + JoyoKanji.size());
            result += BasicLatin;
            result += Hiragana;
            result += Katakana;
            result += JapaneseSymbols;
            result += JoyoKanji;
            return result;
        }

        static constexpr uint32_t AtlasWidth = 1024;

        static constexpr uint32_t AtlasHeight = 1024;

        static auto Create() -> std::unique_ptr<Font::Atlas>;

        Atlas(ID2D1DeviceContext3* deviceContext, IDWriteFactory3* dwriteFactory, IDWriteFontFace3* fontFace, float emSize, std::u32string_view charset = U"");

        [[nodiscard]]
        auto GetGlyph(char32_t codePage) const -> const Glyph*;

        [[nodiscard]]
        auto GetBitmap() const -> ID2D1Bitmap1*
        {
            return m_Bitmap.get();
        }

        [[nodiscard]]
        auto GetLineHeight() const -> float
        {
            return m_LineHeight;
        }

    private:
        wil::com_ptr<ID2D1Bitmap1> m_Bitmap;

        std::unordered_map<char32_t, Glyph> m_Glyphs;

        float m_LineHeight = 0.0f;

        std::u32string m_Charset;
    };

} // namespace N503::Renderer2D::Canvas::Font
