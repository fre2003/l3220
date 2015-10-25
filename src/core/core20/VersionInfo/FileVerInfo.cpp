
/** 
 * 
 * Returns information from specified block.
 * 
 * @param       lpctszBlockString_i - Block
 * @param       ppvData_io          - Data to return
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::ExtractInfo( LPCTSTR lpctszBlockString_i, LPVOID* ppvData_io ) const
{
    if( !lpctszBlockString_i || !ppvData_io || !m_pVersionData || !m_pLang )
    {
        return false;
    }

    CString csBlock;
    csBlock.Format( _T( "\\StringFileInfo\\%04X%04X\\%s" ), 
                        m_pLang[0].wLanguage, 
                        m_pLang[0].wCodePage,
                        lpctszBlockString_i );

    UINT uVersionDataLength = 0;
    const BOOL bResult = VerQueryValue( m_pVersionData, 
                                        csBlock.GetBuffer( 0 ),
                                        ppvData_io,
                                        &uVersionDataLength );
    csBlock.ReleaseBuffer();
    return uVersionDataLength != 0 && bResult != 0;
}

/** 
 * 
 * Extracts string type of information from file.
 * 
 * @param       lpctszBlockString_o - Identifies block.
 * @param       csInfo_o            - On return will hold information
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::ExtractStringInfo( LPCTSTR lpctszBlockString_o, CString& csInfo_o ) const
{
    LPTSTR lptszInfo = 0;
    
    // Extract file description information
    if( !ExtractInfo( lpctszBlockString_o, ( LPVOID* )&lptszInfo ))
    {
        return false;
    }

    // Return value
    csInfo_o = lptszInfo;
    return true;
}// End ExtractStringInfo


/** 
 * 
 * Returns information from specified block.
 * 
 * @param       lpctszBlockString_i - Block
 * @param       ppvData_io          - Data to return
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::ExtractInfo( LPCTSTR lpctszBlockString_i, LPVOID* ppvData_io ) const
{
    if( !lpctszBlockString_i || !ppvData_io || !m_pVersionData || !m_pLang )
    {
        return false;
    }

    CString csBlock;
    csBlock.Format( _T( "\\StringFileInfo\\%04X%04X\\%s" ), 
                        m_pLang[0].wLanguage, 
                        m_pLang[0].wCodePage,
                        lpctszBlockString_i );

    UINT uVersionDataLength = 0;
    const BOOL bResult = VerQueryValue( m_pVersionData, 
                                        csBlock.GetBuffer( 0 ),
                                        ppvData_io,
                                        &uVersionDataLength );
    csBlock.ReleaseBuffer();
    return uVersionDataLength != 0 && bResult != 0;
}

