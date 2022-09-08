using System.IO;

namespace Rose
{
    public class Texture2D
    {
        public Texture2D(string path)
        {
            if (path == null)
                return;

            if (InternalCalls.Asset_GetOrLoadTexture2D(path))
                m_Path = path;
        }
        public bool IsLoaded
        {
            get
            {
                if (m_Path == null)
                    return false;

                return InternalCalls.Asset_GetTexture2D(m_Path);
            }
        }
        internal string m_Path = null;
    }
}

