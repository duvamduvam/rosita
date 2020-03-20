package fr.duvam.video;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

import org.apache.log4j.Logger;

public class MediaManager {

	private static final Logger LOGGER = Logger.getLogger(MediaManager.class);

	List<String> audios = new LinkedList<String>();

	LinkedList<RositaMedia> medias = new LinkedList<RositaMedia>();

	private final static String MEDIA_FILE = "keys/medias.txt";

	private final static String AUDIO_PATH = "audios/";

	public final static String KEY_VIDEO_BASE = "10000";
	public final static String KEY_VIDEO_SPEAK = "10001";

	enum Type {
		GIF, VIDEO, VIDEOR, SPEAK, AUDIO_VIDEO, ARDUINO, LIGHTS;

		public static Type getType(String type) {
			if (VIDEO.toString().equals(type)) {
				return VIDEO;
			} else if (VIDEOR.toString().equals(type)) {
				return VIDEOR;
			} else if (SPEAK.toString().equals(type)) {
				return SPEAK;
			} else if (GIF.toString().equals(type)) {
				return GIF;
			} else if (AUDIO_VIDEO.toString().equals(type)) {
				return AUDIO_VIDEO;
			} else if (ARDUINO.toString().equals(type)) {
				return ARDUINO;
			} else if (LIGHTS.toString().equals(type)) {
				return LIGHTS;
			}else {
				return null;
			}
		}
	}

	enum Control {
		NEXT("10009"), CURRENT("10007"), PREVIOUS("10008");

		Control(String key) {
			this.key = key;
		}

		private final String key;

		public String getKey() {
			return key;
		}
	}

	private int currentAudio = 0;

	public MediaManager() {

		loadMediaFile();
	}

	private void loadMediaFile() {
		File file = new File(MEDIA_FILE);

		try {
			Scanner s = new Scanner(file);
			while (s.hasNext()) {
				RositaMedia media = getMediaFromLine(s.next());
				if (media != null) {
					medias.add(media);
				}
			}
			s.close();
		} catch (FileNotFoundException e) {
			LOGGER.error("can't find " + MEDIA_FILE, e);
		}

	}

	private RositaMedia getMediaFromLine(String str) {

		if (str.contains("|")) {
			Scanner sc = new Scanner(str);
			sc.useDelimiter("[|]");

			RositaMedia media = new RositaMedia(sc.next(), Type.getType(sc.next()), sc.next(), sc.next());

			sc.close();
			return media;
		} else {
			return null;
		}

	}

	public RositaMedia getMedia(String key) {
		for (RositaMedia media : medias) {
			if (key.equals(media.getKey())) {
				return media;
			}
		}
		return null;
	}

	public String getVideo(String key) {

		for (RositaMedia media : medias) {
			if (media.type.equals(Type.VIDEO) && media.getKey().equals(key)) {
				return media.getVideo();
			}
		}
		LOGGER.error("video key " + key + " not found");
		return getVideo(KEY_VIDEO_BASE);
	}

	public String getAudioNavigation(Control control) {
		String audio = null;
		switch (control) {
		case NEXT:
			if ((currentAudio + 1) < audios.size()) {
				audio = audios.get(++currentAudio);
			} else {
				audio = audios.get(currentAudio);
			}
			break;
		case CURRENT:
			if (currentAudio <= audios.size()) {
				audio = audios.get(currentAudio);
			}
			break;
		case PREVIOUS:
			if (currentAudio > 0) {
				audio = audios.get(--currentAudio);
			} else {
				audio = audios.get(currentAudio);
			}
		}
		return AUDIO_PATH + audio;
	}
}