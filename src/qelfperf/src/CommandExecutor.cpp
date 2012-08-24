/*
 * Copyright © 2012 OSLL osll@osll.spb.ru
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */
/*! ---------------------------------------------------------------
 * \file CommandExecutor.cpp
 * \brief CommandExecutor implementation
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "CommandExecutor.h"
#include <QDebug>
#include <stdio.h>

CommandExecutor::CommandExecutor(const QString &cmdName) :
    m_cmdName(cmdName), m_cmdOutput(QSharedPointer<QStringList>(NULL))
{
}

QString CommandExecutor::getCmdName() const
{
    return m_cmdName;
}

void CommandExecutor::setCmdName(const QString &cmdName)
{
    m_cmdName = cmdName;
}

QSharedPointer<QStringList> CommandExecutor::getOutput() const
{
    return m_cmdOutput;
}

int CommandExecutor::exec()
{
    FILE* stream = popen(m_cmdName.toStdString().c_str(), "r");
    m_cmdOutput.clear();
    if (stream != NULL) {
        m_cmdOutput = QSharedPointer<QStringList>(new QStringList());
        QString line;

        char buffer[BUFSIZ + 1];
        long rSize = 1;
        do {
            memset(buffer, '\0', sizeof(buffer));
            rSize = fread(buffer, sizeof(char), BUFSIZ, stream);
            for (int i = 0; i < rSize; i++) {
                line += buffer[i];
                if (buffer[i] == '\n') {
                    *m_cmdOutput << line;
                    line.clear();
                }
            }
        }
        while(rSize != 0);

        if (!line.isEmpty()) {
           *m_cmdOutput << line;
        }

        return 0;
    } else {
        pclose(stream);
        return 1;
    }
}
