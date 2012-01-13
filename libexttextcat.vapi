[CCode (cprefix = "textcat_", lower_case_cprefix = "textcat_")]
namespace TextCat {
	[Compact]
	[CCode (cname="candidate_t",cheader_filename = "textcat.h")]
	public struct candidate {
		public weak string name;
		public int score;
	}
	[Compact]
	[CCode (cname="void",cheader_filename = "textcat.h", free_function="textcat_Done")]
	public class Classifier {
		[CCode (cname = "textcat_Classify", cheader_filename = "textcat.h")]
		public unowned string classify (string buffer, size_t size);
		[CCode (cname = "textcat_ClassifyFull", cheader_filename = "textcat.h")]
		public int classify_full (string buffer, size_t size, candidate* candidates);
		[CCode (cname = "textcat_GetClassifyFullOutput", cheader_filename = "textcat.h")]
		public unowned candidate* get_classify_full_output ();
		[CCode (cname = "textcat_ReleaseClassifyFullOutput", cheader_filename = "textcat.h")]
		public void release_classify_full_output (candidate* candidates);
		[CCode (cname = "textcat_Init", cheader_filename = "textcat.h")]
		public Classifier (string conffile);
		[CCode (cname = "textcat_SetProperty", cheader_filename = "textcat.h")]
		public int set_property (Property property, int4 value);
		
	}
	[CCode (cname="textcat_Property",cheader_filename = "textcat.h",cprefix = "TCPROP_")]
	public enum Property {
		UTF8AWARE;
	}
	[CCode (cheader_filename = "textcat.h")]
	public const int TEXTCAT_RESULT_SHORT;
	[CCode (cheader_filename = "textcat.h")]
	public const int TEXTCAT_RESULT_UNKOWN;
	[CCode (cname = "textcat_Version", cheader_filename = "textcat.h")]
	public static unowned string version ();
}
